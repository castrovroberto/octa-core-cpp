/**
 * @file IGameMap.h
 * @brief Interface definition for the game map abstraction
 * 
 * This file defines the IGameMap interface, which abstracts the underlying
 * data structure used to store and access game cells. This abstraction allows
 * the game engine to function independently of whether the map is stored as
 * a graph, array, or any other structure.
 * 
 * Updated in Phase P1.2 to conform to SRD v3.2 specification with simplified
 * interface using at() and size() methods.
 */

#ifndef OCTA_CORE_IGAMEMAP_H
#define OCTA_CORE_IGAMEMAP_H

#include <memory>
#include "../model/Coordinate.h"
#include "../core/GameCell.h"

// Forward declaration
class GameCell;

/**
 * @class IGameMap
 * @brief Abstract interface for the game map
 *
 * This abstract base class defines the essential operations for a game map,
 * allowing the game engine to function independently of the underlying
 * data structure implementation.
 * 
 * The interface has been simplified in SRD v3.2 to focus on the core
 * operations needed by the game logic:
 * - Cell access by coordinate
 * - Total cell count for game state queries
 * 
 * Concrete implementations include:
 * - GraphGameMap: Hash map-based implementation for sparse grids
 * - ArrayGameMap: 2D array-based implementation (future)
 */
class IGameMap {
public:
    /**
     * @brief Virtual destructor for polymorphic base class
     * 
     * Essential for proper cleanup when deleting through base pointer.
     */
    virtual ~IGameMap() = default;

    /**
     * @brief Retrieves a cell at a specific coordinate
     * @param coord The coordinate of the cell to retrieve
     * @return Shared pointer to the GameCell, or nullptr if no cell exists at that coordinate
     * 
     * This is the primary access method for retrieving cells from the map.
     * The method name follows SRD v3.2 specification.
     */
    virtual std::shared_ptr<GameCell> at(const Coordinate& coord) const = 0;

    /**
     * @brief Gets the total number of cells in the map
     * @return The total count of cells in this map
     * 
     * This method returns the actual number of instantiated cells,
     * which is useful for game state queries, win condition checks,
     * and performance monitoring.
     */
    virtual size_t size() const = 0;
};

#endif // OCTA_CORE_IGAMEMAP_H 