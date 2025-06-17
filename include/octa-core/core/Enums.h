/**
 * @file Enums.h
 * @brief Core enumerations for the Octa-Core game engine
 * 
 * This file defines the fundamental enum types used throughout the game:
 * - CellState: Represents the ownership/state of a game cell
 * - Player: Represents the two players in the game
 * 
 * These enums are specified in SRD v3.2 and form the foundation for 
 * the game's state management system.
 */

#ifndef OCTA_CORE_ENUMS_H
#define OCTA_CORE_ENUMS_H

#include <cstdint>
#include <optional>

/**
 * @enum CellState
 * @brief Defines the state/ownership of a game cell
 * 
 * Each cell on the game map can be in one of four states:
 * - NEUTRAL: Unowned, can be captured by either player
 * - PLAYER_1: Owned by Player 1
 * - PLAYER_2: Owned by Player 2  
 * - BLOCKED: Permanently blocked, cannot be captured
 */
enum class CellState : uint8_t {
    NEUTRAL,   ///< Cell is unowned and available for capture
    PLAYER_1,  ///< Cell is owned by Player 1
    PLAYER_2,  ///< Cell is owned by Player 2
    BLOCKED    ///< Cell is permanently blocked
};

/**
 * @enum Player
 * @brief Represents the two players in the game
 * 
 * The game supports exactly two players who alternate turns.
 * This enum is used for turn management and ownership validation.
 */
enum class Player : uint8_t {
    PLAYER_1,  ///< First player (typically starts the game)
    PLAYER_2   ///< Second player
};

/**
 * @brief Converts a Player enum to the corresponding CellState
 * @param player The player to convert
 * @return The corresponding CellState for the player
 */
inline CellState playerToCellState(Player player) {
    switch (player) {
        case Player::PLAYER_1:
            return CellState::PLAYER_1;
        case Player::PLAYER_2:
            return CellState::PLAYER_2;
        default:
            // This should never happen with a well-formed Player enum
            return CellState::NEUTRAL;
    }
}

/**
 * @brief Converts a CellState to the corresponding Player enum
 * @param state The cell state to convert
 * @return The corresponding Player, or nullopt if state is not player-owned
 */
inline std::optional<Player> cellStateToPlayer(CellState state) {
    switch (state) {
        case CellState::PLAYER_1:
            return Player::PLAYER_1;
        case CellState::PLAYER_2:
            return Player::PLAYER_2;
        default:
            return std::nullopt;
    }
}

/**
 * @brief Gets the opponent of the given player
 * @param player The current player
 * @return The opposing player
 */
inline Player getOpponent(Player player) {
    return (player == Player::PLAYER_1) ? Player::PLAYER_2 : Player::PLAYER_1;
}

#endif // OCTA_CORE_ENUMS_H 