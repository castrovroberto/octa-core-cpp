#ifndef OCTA_CORE_IGAMEMAP_H
#define OCTA_CORE_IGAMEMAP_H

#include <vector>
#include <memory>
#include "../model/Coordinate.h"
#include "../core/GameCell.h"

// Forward declaration
class GameCell;

/**
 * @class IGameMap
 * @brief An interface for the game map, abstracting the underlying data structure.
 *
 * This abstract base class defines the essential operations for a game map,
 * allowing the game engine to function independently of whether the map is
 * stored as a graph, an array, or any other structure.
 */
class IGameMap {
public:
    // Virtual destructor is essential for a polymorphic base class.
    virtual ~IGameMap() = default;

    /**
     * @brief Retrieves a cell at a specific coordinate.
     * @param coord The coordinate of the cell to retrieve.
     * @return A shared pointer to the GameCell, or nullptr if no cell exists.
     */
    virtual std::shared_ptr<GameCell> getCell(const Coordinate& coord) const = 0;

    /**
     * @brief Retrieves a list of all cells in the map.
     * @return A vector of shared pointers to all GameCells.
     */
    virtual std::vector<std::shared_ptr<GameCell>> getAllCells() const = 0;

    /**
     * @brief Gets the size (radius) of the map.
     * @return The integer size of the map.
     */
    virtual int getSize() const = 0;

    /**
     * @brief Gets the total number of cells in the map.
     * @return The total cell count.
     */
    virtual int getTotalCellCount() const = 0;
};

#endif // OCTA_CORE_IGAMEMAP_H 