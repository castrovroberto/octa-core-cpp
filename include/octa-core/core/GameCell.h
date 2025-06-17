#ifndef OCTA_CORE_GAMECELL_H
#define OCTA_CORE_GAMECELL_H

#include "../model/Coordinate.h"
#include <vector>
#include <memory>

// Forward declaration to avoid circular dependencies
class GameCell;

/**
 * @enum Direction
 * @brief Represents the 6 directions in a hexagonal grid.
 */
enum class Direction {
    NORTH_EAST,
    EAST,
    SOUTH_EAST,
    SOUTH_WEST,
    WEST,
    NORTH_WEST
};

/**
 * @enum Player
 * @brief Represents the players in the game.
 */
enum class Player {
    NONE,       // No player occupies this cell
    PLAYER_1,   // Player 1
    PLAYER_2    // Player 2
};

/**
 * @class GameCell
 * @brief Represents a single cell on the hexagonal game map.
 *
 * Each cell has a coordinate position, can be occupied by a player,
 * and maintains references to its neighbors in the hexagonal grid.
 */
class GameCell {
public:
    /**
     * @brief Constructor for GameCell.
     * @param coordinate The position of this cell on the map.
     */
    explicit GameCell(const Coordinate& coordinate);

    /**
     * @brief Gets the coordinate of this cell.
     * @return The coordinate position.
     */
    const Coordinate& getCoordinate() const { return coordinate_; }

    /**
     * @brief Gets the player occupying this cell.
     * @return The player (NONE if unoccupied).
     */
    Player getOccupant() const { return occupant_; }

    /**
     * @brief Sets the player occupying this cell.
     * @param player The player to occupy this cell.
     */
    void setOccupant(Player player) { occupant_ = player; }

    /**
     * @brief Checks if this cell is occupied by any player.
     * @return True if occupied, false otherwise.
     */
    bool isOccupied() const { return occupant_ != Player::NONE; }

    /**
     * @brief Gets a neighbor in the specified direction.
     * @param direction The direction to look for a neighbor.
     * @return Shared pointer to the neighbor cell, or nullptr if no neighbor exists.
     */
    std::shared_ptr<GameCell> getNeighbor(Direction direction) const;

    /**
     * @brief Sets a neighbor in the specified direction.
     * @param direction The direction to set the neighbor.
     * @param neighbor The neighbor cell to set.
     */
    void setNeighbor(Direction direction, std::shared_ptr<GameCell> neighbor);

    /**
     * @brief Gets all neighbors of this cell.
     * @return Vector of shared pointers to neighboring cells (may contain nullptrs).
     */
    std::vector<std::shared_ptr<GameCell>> getAllNeighbors() const;

    /**
     * @brief Gets all valid neighbors (non-null) of this cell.
     * @return Vector of shared pointers to valid neighboring cells.
     */
    std::vector<std::shared_ptr<GameCell>> getValidNeighbors() const;

private:
    Coordinate coordinate_;
    Player occupant_;
    std::vector<std::shared_ptr<GameCell>> neighbors_;  // Index corresponds to Direction enum values

    static constexpr size_t NUM_DIRECTIONS = 6;
};

#endif // OCTA_CORE_GAMECELL_H 