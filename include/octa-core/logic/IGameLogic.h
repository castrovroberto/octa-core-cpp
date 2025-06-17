/**
 * @file IGameLogic.h
 * @brief Interface definition for game logic implementations
 * 
 * This file defines the IGameLogic interface, which abstracts the core
 * game mechanics and rule enforcement. This interface allows different
 * game logic implementations to be used interchangeably with the game engine.
 * 
 * Created in Phase P1.3 according to SRD v3.2 specifications.
 */

#ifndef OCTA_CORE_IGAMELOGIC_H
#define OCTA_CORE_IGAMELOGIC_H

#include <memory>
#include "../core/GameCell.h"
#include "../core/Enums.h"
#include "../model/GameConfig.h"

// Forward declarations
class GameCell;

/**
 * @class IGameLogic
 * @brief Abstract interface for game logic implementations
 * 
 * This interface defines the essential operations for game logic,
 * including move validation, move execution, game state management,
 * and win condition checking.
 * 
 * The interface is designed to be:
 * - Stateful: Maintains current player, turn count, and game state
 * - Configurable: Uses GameConfig for customizable behavior
 * - Exception-safe: Provides validation and error handling
 * - Testable: Clear separation of concerns for unit testing
 * 
 * Concrete implementations include:
 * - OctaGameLogic: Full implementation with chain reaction mechanics
 * - MockGameLogic: Test implementation for unit testing
 */
class IGameLogic {
public:
    /**
     * @brief Virtual destructor for polymorphic base class
     * 
     * Essential for proper cleanup when deleting through base pointer.
     */
    virtual ~IGameLogic() = default;

    /**
     * @brief Validates whether a move is legal for the current player
     * @param cell The cell where the player wants to make a move
     * @param player The player attempting to make the move
     * @return True if the move is valid, false otherwise
     * 
     * This method performs validation without modifying game state.
     * A move is typically valid if:
     * - The cell is owned by the specified player
     * - The game is not over
     * - It's the specified player's turn
     * 
     * This method should be const and have no side effects.
     */
    virtual bool isValidMove(std::shared_ptr<const GameCell> cell, Player player) const = 0;

    /**
     * @brief Executes a move and returns the result
     * @param cell The cell where the player is making a move
     * @param player The player making the move
     * @return GameResult indicating the outcome of the move
     * @throws std::invalid_argument If the move is invalid
     * @throws std::runtime_error If game state is inconsistent
     * 
     * This method:
     * 1. Validates the move (throws if invalid)
     * 2. Executes the chain reaction algorithm
     * 3. Updates game state (turn count, current player)
     * 4. Checks for win conditions
     * 5. Returns the current game result
     * 
     * The method provides exception safety according to the configured SafetyLevel.
     */
    virtual GameResult makeMove(std::shared_ptr<GameCell> cell, Player player) = 0;

    /**
     * @brief Checks if the game has ended
     * @return True if the game is over, false otherwise
     * 
     * The game can end due to:
     * - Win condition being met (elimination, turn limit)
     * - Explicit game termination
     * - Error conditions that prevent continuation
     * 
     * This method should be const and have no side effects.
     */
    virtual bool isGameOver() const = 0;

    /**
     * @brief Gets the player whose turn it currently is
     * @return The current active player
     * 
     * This is used by the game engine to determine whose move to request
     * and for UI display purposes.
     */
    virtual Player getCurrentPlayer() const = 0;

    /**
     * @brief Switches to the next player's turn
     * 
     * This method is typically called after a successful move to advance
     * the turn to the opponent. The implementation should toggle between
     * Player::PLAYER_1 and Player::PLAYER_2.
     * 
     * This method should not be called if the game is over.
     */
    virtual void switchPlayer() = 0;

    /**
     * @brief Gets the current turn number
     * @return The number of turns completed so far
     * 
     * Turn counting typically starts at 0 and increments after each
     * player's move. This is used for:
     * - Turn limit win conditions
     * - Game statistics and logging
     * - UI display
     */
    virtual int getTurnCount() const = 0;

    /**
     * @brief Gets the game configuration
     * @return Reference to the current game configuration
     * 
     * The configuration controls game behavior such as win conditions,
     * turn limits, and safety levels. This method provides read-only
     * access to the current settings.
     */
    virtual const GameConfig& getConfig() const = 0;

    /**
     * @brief Gets the current game result (if available)
     * @return Optional GameResult, nullopt if game is not over
     * 
     * This method returns the current game state as a result object.
     * If the game is not over, it returns nullopt. If the game has ended,
     * it returns a GameResult with winner information and statistics.
     * 
     * This is useful for:
     * - Checking final game state
     * - Displaying results to users
     * - Recording game statistics
     */
    virtual std::optional<GameResult> getGameResult() const = 0;

    /**
     * @brief Resets the game to initial state
     * @param newConfig Optional new configuration to use
     * 
     * This method resets all game state to the beginning:
     * - Turn count back to 0
     * - Current player back to PLAYER_1
     * - Game result cleared
     * - All cells reset to initial state (if applicable)
     * 
     * If a new configuration is provided, it replaces the current one.
     * This is useful for starting new games without recreating objects.
     */
    virtual void resetGame(const GameConfig* newConfig = nullptr) = 0;
};

#endif // OCTA_CORE_IGAMELOGIC_H 