/**
 * @file GameCell.cpp
 * @brief Implementation of the GameCell class for the Octa-Core game engine
 * 
 * This file implements the GameCell class methods, providing functionality
 * for managing cell state, neighbor relationships, and octagonal grid navigation.
 * 
 * Refactored in Phase P1.1 to use the new enum system and weak_ptr
 * neighbor references.
 */

#include "octa-core/core/GameCell.h"

GameCell::GameCell(const Coordinate& coordinate, CellState initialState)
    : coordinate_(coordinate), state_(initialState), direction_(Direction::N), value_(0) {
    // Initialize all neighbors as empty weak_ptr instances
    // No explicit initialization needed for std::array of weak_ptr
}

bool GameCell::isOwnedByPlayer() const {
    return state_ == CellState::PLAYER_1 || state_ == CellState::PLAYER_2;
}

bool GameCell::isOwnedBy(Player player) const {
    return state_ == playerToCellState(player);
}

std::shared_ptr<GameCell> GameCell::getNeighbor(Direction direction) const {
    size_t index = static_cast<size_t>(direction);
    if (index < NUM_DIRECTIONS) {
        return neighbors_[index].lock();  // Convert weak_ptr to shared_ptr
    }
    return nullptr;
}

void GameCell::setNeighbor(Direction direction, std::shared_ptr<GameCell> neighbor) {
    size_t index = static_cast<size_t>(direction);
    if (index < NUM_DIRECTIONS) {
        neighbors_[index] = neighbor;  // Store as weak_ptr
    }
}

void GameCell::clearNeighbor(Direction direction) {
    size_t index = static_cast<size_t>(direction);
    if (index < NUM_DIRECTIONS) {
        neighbors_[index].reset();  // Clear the weak_ptr
    }
}

std::vector<std::shared_ptr<GameCell>> GameCell::getValidNeighbors() const {
    std::vector<std::shared_ptr<GameCell>> validNeighbors;
    validNeighbors.reserve(NUM_DIRECTIONS);  // Pre-allocate for efficiency
    
    for (const auto& weakNeighbor : neighbors_) {
        if (auto sharedNeighbor = weakNeighbor.lock()) {
            validNeighbors.push_back(sharedNeighbor);
        }
    }
    
    return validNeighbors;
}

std::vector<std::shared_ptr<GameCell>> GameCell::getNeighborsInDirections(const std::vector<Direction>& directions) const {
    std::vector<std::shared_ptr<GameCell>> neighbors;
    neighbors.reserve(directions.size());
    
    for (Direction direction : directions) {
        neighbors.push_back(getNeighbor(direction));
    }
    
    return neighbors;
}

size_t GameCell::getValidNeighborCount() const {
    size_t count = 0;
    for (const auto& weakNeighbor : neighbors_) {
        if (!weakNeighbor.expired()) {
            ++count;
        }
    }
    return count;
}

bool GameCell::hasNeighbor(Direction direction) const {
    size_t index = static_cast<size_t>(direction);
    if (index < NUM_DIRECTIONS) {
        return !neighbors_[index].expired();
    }
    return false;
} 