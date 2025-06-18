/**
 * @file GameCell.h
 * @brief Definition of the GameCell class for the Octa-Core game engine
 *
 * This file defines the GameCell class, which represents a single cell
 * in the octagonal game map. Each cell has a coordinate position, a state
 * (neutral, player-owned, or blocked), and maintains weak references to
 * its neighbors in the octagonal grid.
 *
 * Refactored in Phase P1.1 to use the new enum system and weak_ptr
 * neighbor references to avoid circular dependencies.
 */

#ifndef OCTA_CORE_GAMECELL_H
#define OCTA_CORE_GAMECELL_H

#include "Direction.h"
#include "Enums.h"

#include <array>
#include <memory>
#include <vector>

#include "../model/Coordinate.h"

// Forward declaration to avoid circular dependencies
class GameCell;

/**
 * @class GameCell
 * @brief Represents a single cell on the octagonal game map.
 *
 * Each cell has a coordinate position, a state indicating ownership or
 * availability, and maintains weak references to its neighbors in the
 * octagonal grid. Weak references are used to prevent circular dependencies
 * and simplify memory management.
 *
 * Key features:
 * - Uses CellState enum for ownership/availability tracking
 * - Maintains 8 directional neighbors using weak_ptr references
 * - Supports efficient neighbor lookups and traversal
 * - Direction-aware neighbor management for octagonal grid navigation
 */
class GameCell {
  public:
    /**
     * @brief Constructor for GameCell.
     * @param coordinate The position of this cell on the map.
     * @param initialState The initial state of the cell (defaults to NEUTRAL).
     */
    explicit GameCell(const Coordinate& coordinate, CellState initialState = CellState::NEUTRAL);

    /**
     * @brief Gets the coordinate of this cell.
     * @return The coordinate position.
     */
    const Coordinate& getCoordinate() const { return coordinate_; }

    /**
     * @brief Gets the current state of this cell.
     * @return The cell state (NEUTRAL, PLAYER_1, PLAYER_2, or BLOCKED).
     */
    CellState getState() const { return state_; }

    /**
     * @brief Sets the state of this cell.
     * @param state The new cell state.
     */
    void setState(CellState state) { state_ = state; }

    /**
     * @brief Checks if this cell is owned by any player.
     * @return True if owned by a player, false otherwise.
     */
    bool isOwnedByPlayer() const;

    /**
     * @brief Checks if this cell is owned by a specific player.
     * @param player The player to check.
     * @return True if owned by the specified player, false otherwise.
     */
    bool isOwnedBy(Player player) const;

    /**
     * @brief Checks if this cell is available for capture.
     * @return True if the cell is NEUTRAL, false otherwise.
     */
    bool isAvailable() const { return state_ == CellState::NEUTRAL; }

    /**
     * @brief Checks if this cell is permanently blocked.
     * @return True if the cell is BLOCKED, false otherwise.
     */
    bool isBlocked() const { return state_ == CellState::BLOCKED; }

    /**
     * @brief Gets the current direction (used for directional properties).
     * @return The current direction value.
     */
    Direction getDirection() const { return direction_; }

    /**
     * @brief Sets the direction for this cell.
     * @param direction The new direction value.
     */
    void setDirection(Direction direction) { direction_ = direction; }

    /**
     * @brief Gets the current value/energy level of this cell.
     * @return The current value of the cell.
     */
    int getValue() const { return value_; }

    /**
     * @brief Sets the value/energy level of this cell.
     * @param value The new value for the cell.
     */
    void setValue(int value) { value_ = value; }

    /**
     * @brief Gets a neighbor in the specified direction.
     * @param direction The direction to look for a neighbor.
     * @return Shared pointer to the neighbor cell, or nullptr if no neighbor exists or if the
     * weak_ptr has expired.
     */
    std::shared_ptr<GameCell> getNeighbor(Direction direction) const;

    /**
     * @brief Sets a neighbor in the specified direction.
     * @param direction The direction to set the neighbor.
     * @param neighbor The neighbor cell to set.
     */
    void setNeighbor(Direction direction, std::shared_ptr<GameCell> neighbor);

    /**
     * @brief Removes the neighbor reference in the specified direction.
     * @param direction The direction to clear.
     */
    void clearNeighbor(Direction direction);

    /**
     * @brief Gets all neighbors of this cell.
     * @return Array of weak pointers to neighboring cells (may contain expired pointers).
     */
    const std::array<std::weak_ptr<GameCell>, NUM_DIRECTIONS>& getAllNeighborRefs() const {
        return neighbors_;
    }

    /**
     * @brief Gets all valid neighbors (non-expired) of this cell as shared pointers.
     * @return Vector of shared pointers to valid neighboring cells.
     */
    std::vector<std::shared_ptr<GameCell>> getValidNeighbors() const;

    /**
     * @brief Gets all neighbors in the specified directions.
     * @param directions Vector of directions to check.
     * @return Vector of shared pointers to neighbor cells (may contain nullptrs for invalid/expired
     * neighbors).
     */
    std::vector<std::shared_ptr<GameCell>>
    getNeighborsInDirections(const std::vector<Direction>& directions) const;

    /**
     * @brief Counts the number of valid (non-expired) neighbors.
     * @return Number of valid neighbors.
     */
    size_t getValidNeighborCount() const;

    /**
     * @brief Checks if this cell has a valid neighbor in the specified direction.
     * @param direction The direction to check.
     * @return True if a valid neighbor exists, false otherwise.
     */
    bool hasNeighbor(Direction direction) const;

  private:
    Coordinate coordinate_;  ///< Position of this cell on the map
    CellState state_;        ///< Current state/ownership of the cell
    Direction direction_;    ///< Current direction property for this cell
    int value_;              ///< Current value/energy level of the cell
    std::array<std::weak_ptr<GameCell>, NUM_DIRECTIONS>
        neighbors_;  ///< Neighbors in all 8 directions

    static constexpr size_t NUM_DIRECTIONS = 8;
};

#endif  // OCTA_CORE_GAMECELL_H