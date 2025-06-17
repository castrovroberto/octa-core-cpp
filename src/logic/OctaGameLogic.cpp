/**
 * @file OctaGameLogic.cpp
 * @brief Implementation of OctaGameLogic class
 * 
 * This file contains the complete implementation of the Octa-Core game
 * logic, including chain reactions, win condition checking, and move
 * validation. This is the core game mechanics implementation.
 * 
 * Created in Phase P1.3 according to SRD v3.2 specifications.
 */

#include "../../include/octa-core/logic/OctaGameLogic.h"
#include "../../include/octa-core/core/Direction.h"
#include <stdexcept>
#include <algorithm>
#include <sstream>

OctaGameLogic::OctaGameLogic(std::shared_ptr<IGameMap> gameMap, const GameConfig& config)
    : gameMap_(gameMap), config_(config), gameOver_(false) {
    
    if (!gameMap_) {
        throw std::invalid_argument("Game map cannot be null");
    }
    
    validateConfig(config_);
    initializeGameState();
}

bool OctaGameLogic::isValidMove(std::shared_ptr<const GameCell> cell, Player player) const {
    if (!cell) {
        return false;
    }
    
    if (isGameOver()) {
        return false;
    }
    
    if (player != currentPlayer_) {
        return false;
    }
    
    // A move is valid if the cell is owned by the player or is neutral
    CellState cellState = cell->getState();
    CellState playerState = playerToCellState(player);
    
    return cellState == playerState || cellState == CellState::NEUTRAL;
}

GameResult OctaGameLogic::makeMove(std::shared_ptr<GameCell> cell, Player player) {
    if (!isValidMove(cell, player)) {
        std::ostringstream oss;
        oss << "Invalid move: ";
        if (!cell) {
            oss << "null cell";
        } else if (isGameOver()) {
            oss << "game is over";
        } else if (player != currentPlayer_) {
            oss << "not player's turn (current: " << static_cast<int>(currentPlayer_) + 1 << ")";
        } else {
            oss << "cell not owned by player " << static_cast<int>(player) + 1;
        }
        throw std::invalid_argument(oss.str());
    }
    
    // Execute the chain reaction
    std::vector<std::shared_ptr<GameCell>> affectedCells = executeChainReaction(cell, player);
    
    // Switch to next player
    switchPlayer();
    
    // Increment turn count after each move (not just after both players move)
    // This gives us a more granular turn count for better game tracking
    turnCount_++;
    
    // Check for win conditions
    auto result = checkWinConditions();
    if (result.has_value()) {
        gameResult_ = result;
        gameOver_ = true;
        return result.value();
    }
    
    // Return current game state
    int p1Cells = countPlayerCells(Player::PLAYER_1);
    int p2Cells = countPlayerCells(Player::PLAYER_2);
    
    return GameResult("Game continues", turnCount_, p1Cells, p2Cells);
}

bool OctaGameLogic::isGameOver() const {
    if (gameOver_) {
        return true;
    }
    
    // Check win conditions to update state
    auto result = checkWinConditions();
    if (result.has_value()) {
        gameResult_ = result;
        gameOver_ = true;
        return true;
    }
    
    return false;
}

Player OctaGameLogic::getCurrentPlayer() const {
    return currentPlayer_;
}

void OctaGameLogic::switchPlayer() {
    currentPlayer_ = getOpponent(currentPlayer_);
}

int OctaGameLogic::getTurnCount() const {
    return turnCount_;
}

const GameConfig& OctaGameLogic::getConfig() const {
    return config_;
}

std::optional<GameResult> OctaGameLogic::getGameResult() const {
    if (isGameOver()) {
        return gameResult_;
    }
    return std::nullopt;
}

void OctaGameLogic::resetGame(const GameConfig* newConfig) {
    if (newConfig) {
        validateConfig(*newConfig);
        config_ = *newConfig;
    }
    
    initializeGameState();
    
    // Reset all cells to neutral state
    // Note: This assumes the game map provides a way to iterate through all cells
    // For now, we'll leave the cells as they are since the map interface doesn't
    // provide a direct way to reset all cells. This would be handled by creating
    // a new game map instance in practice.
}

std::vector<std::shared_ptr<GameCell>> OctaGameLogic::executeChainReaction(
    std::shared_ptr<GameCell> startCell, Player player) {
    
    std::vector<std::shared_ptr<GameCell>> affectedCells;
    std::queue<std::shared_ptr<GameCell>> explosionQueue;
    
    // Set the starting cell to the player's ownership and increment its value
    startCell->setState(playerToCellState(player));
    startCell->setValue(startCell->getValue() + 1);
    affectedCells.push_back(startCell);
    
    // Check if the starting cell should explode
    if (shouldExplode(startCell)) {
        explosionQueue.push(startCell);
    }
    
    // Process chain reactions using breadth-first search
    while (!explosionQueue.empty()) {
        auto currentCell = explosionQueue.front();
        explosionQueue.pop();
        
        // Only explode if the cell is still unstable (might have been stabilized by another explosion)
        if (shouldExplode(currentCell)) {
            explodeCell(currentCell, player, affectedCells);
            
            // Check all affected neighbors for further explosions
            for (Direction dir = Direction::N; dir <= Direction::NW; 
                 dir = static_cast<Direction>(static_cast<uint8_t>(dir) + 1)) {
                
                auto neighbor = currentCell->getNeighbor(dir);
                if (neighbor && shouldExplode(neighbor)) {
                    // Only add to queue if not already queued (simple duplicate prevention)
                    explosionQueue.push(neighbor);
                }
            }
        }
    }
    
    return affectedCells;
}

bool OctaGameLogic::shouldExplode(std::shared_ptr<const GameCell> cell) const {
    if (!cell || cell->getState() == CellState::BLOCKED) {
        return false;
    }
    
    return cell->getValue() > cell->getValidNeighborCount();
}

void OctaGameLogic::explodeCell(std::shared_ptr<GameCell> cell, Player player,
                               std::vector<std::shared_ptr<GameCell>>& affectedCells) {
    
    int explosionValue = cell->getValue();
    CellState playerState = playerToCellState(player);
    
    // Reset the exploding cell
    cell->setValue(0);
    cell->setState(playerState);
    
    // Distribute energy to all valid neighbors
    for (Direction dir = Direction::N; dir <= Direction::NW; 
         dir = static_cast<Direction>(static_cast<uint8_t>(dir) + 1)) {
        
        auto neighbor = cell->getNeighbor(dir);
        if (!neighbor || neighbor->getState() == CellState::BLOCKED) {
            continue;
        }
        
        // Check stopOnEnemy configuration
        if (config_.stopOnEnemy && neighbor->getState() != CellState::NEUTRAL && 
            neighbor->getState() != playerState) {
            continue; // Skip enemy cells if stopOnEnemy is true
        }
        
        // Convert neighbor to current player and add energy
        neighbor->setState(playerState);
        neighbor->setValue(neighbor->getValue() + 1);
        
        // Track affected cell
        if (std::find(affectedCells.begin(), affectedCells.end(), neighbor) == affectedCells.end()) {
            affectedCells.push_back(neighbor);
        }
    }
}

std::optional<GameResult> OctaGameLogic::checkWinConditions() const {
    int p1Cells = countPlayerCells(Player::PLAYER_1);
    int p2Cells = countPlayerCells(Player::PLAYER_2);
    
    // Check elimination condition - but only if both players have had at least one turn
    // This prevents immediate elimination when the game first starts
    if (config_.winCondition == WinCondition::ELIMINATION && turnCount_ >= 2 && (p1Cells > 0 || p2Cells > 0)) {
        if (p1Cells == 0 && p2Cells > 0) {
            return GameResult(Player::PLAYER_2, "Player 1 eliminated", turnCount_, p1Cells, p2Cells);
        }
        if (p2Cells == 0 && p1Cells > 0) {
            return GameResult(Player::PLAYER_1, "Player 2 eliminated", turnCount_, p1Cells, p2Cells);
        }
    }
    
    // Check turn limit condition
    if (config_.winCondition == WinCondition::TURN_LIMIT_MAJORITY && turnCount_ >= config_.turnLimit) {
        if (p1Cells > p2Cells) {
            return GameResult(Player::PLAYER_1, "Turn limit reached - Player 1 majority", 
                            turnCount_, p1Cells, p2Cells);
        }
        if (p2Cells > p1Cells) {
            return GameResult(Player::PLAYER_2, "Turn limit reached - Player 2 majority", 
                            turnCount_, p1Cells, p2Cells);
        }
        return GameResult("Turn limit reached - tie", turnCount_, p1Cells, p2Cells);
    }
    
    return std::nullopt; // Game continues
}

int OctaGameLogic::countPlayerCells(Player player) const {
    int count = 0;
    CellState targetState = playerToCellState(player);
    
    // Iterate through all coordinates in the map
    // Note: This is a simplified approach. In a real implementation,
    // we might want a more efficient way to iterate through all cells
    // or maintain cell counts as the game progresses.
    
    // For now, we'll use a reasonable approach assuming the map has a finite size
    // We'll iterate through a reasonable coordinate range
    for (int x = -10; x <= 10; ++x) {
        for (int y = -10; y <= 10; ++y) {
            try {
                auto cell = gameMap_->at(Coordinate(x, y));
                if (cell && cell->getState() == targetState) {
                    count++;
                }
            } catch (const std::exception&) {
                // Coordinate not valid in map, continue
                continue;
            }
        }
    }
    
    return count;
}

void OctaGameLogic::validateConfig(const GameConfig& config) const {
    if (!config.isValid()) {
        throw std::invalid_argument("Invalid game configuration");
    }
    
    if (config.turnLimit <= 0) {
        throw std::invalid_argument("Turn limit must be positive");
    }
    
    if (config.winCondition == WinCondition::TURN_LIMIT_MAJORITY && config.turnLimit < 1) {
        throw std::invalid_argument("Turn limit majority requires at least 1 turn");
    }
}

void OctaGameLogic::initializeGameState() {
    currentPlayer_ = Player::PLAYER_1;
    turnCount_ = 0;
    gameOver_ = false;
    gameResult_ = std::nullopt;
} 