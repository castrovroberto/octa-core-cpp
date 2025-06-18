/**
 * @file GraphGameMap.h
 * @brief Graph-based implementation of the IGameMap interface
 *
 * This file defines the GraphGameMap class, which implements the IGameMap
 * interface using a hash map (std::unordered_map) for efficient coordinate-based
 * lookups. This implementation is ideal for sparse octagonal grids where cells
 * are connected in a graph-like structure.
 *
 * Updated in Phase P1.2 to implement the new SRD v3.2 interface specification.
 */

#ifndef OCTA_CORE_GRAPHGAMEMAP_H
#define OCTA_CORE_GRAPHGAMEMAP_H

#include "IGameMap.h"

#include <unordered_map>

/**
 * @struct CoordinateHash
 * @brief Custom hash function for Coordinate objects
 *
 * This hash function enables the use of Coordinate objects as keys in
 * std::unordered_map. It combines the hash values of the x and y components
 * using a simple XOR with bit shifting to reduce collisions.
 */
struct CoordinateHash {
    std::size_t operator()(const Coordinate& coord) const {
        // Combine hash values of x and y coordinates
        auto hashX = std::hash<int>()(coord.getX());
        auto hashY = std::hash<int>()(coord.getY());
        return hashX ^ (hashY << 1);
    }
};

/**
 * @class GraphGameMap
 * @brief Graph-based implementation of IGameMap using hash maps
 *
 * This implementation uses an unordered_map for efficient coordinate-based
 * lookups and is ideal for sparse octagonal grids where cells are connected
 * in a graph-like structure.
 *
 * Key features:
 * - O(1) average-case cell lookup by coordinate
 * - Efficient memory usage for sparse grids
 * - Automatic neighbor linking in octagonal pattern
 * - Support for square grid initialization with configurable size
 *
 * The map is initialized as a square grid from (-size, -size) to (size, size),
 * resulting in (2*size+1)² total cells. All cells are automatically linked
 * to their 8 octagonal neighbors using weak_ptr references to prevent
 * circular dependencies.
 */
class GraphGameMap : public IGameMap {
  public:
    /**
     * @brief Constructor that creates an octagonal map of the specified size
     * @param size The radius of the map (distance from center to edge)
     *
     * Creates a square grid with coordinates ranging from (-size, -size) to
     * (size, size), resulting in (2*size+1)² total cells. All cells are
     * initialized with NEUTRAL state and linked to their neighbors.
     */
    explicit GraphGameMap(int size);

    // IGameMap interface implementation

    /**
     * @brief Retrieves a cell at a specific coordinate
     * @param coord The coordinate of the cell to retrieve
     * @return Shared pointer to the GameCell, or nullptr if no cell exists at that coordinate
     */
    std::shared_ptr<GameCell> at(const Coordinate& coord) const override;

    /**
     * @brief Gets the total number of cells in the map
     * @return The total count of cells in this map
     */
    size_t size() const override;

    // Additional utility methods (not part of IGameMap interface)

    /**
     * @brief Gets the radius/size parameter used to create this map
     * @return The size parameter (radius from center to edge)
     *
     * This method is useful for debugging and map introspection but is not
     * part of the IGameMap interface.
     */
    int getRadius() const { return mapSize_; }

    /**
     * @brief Checks if a coordinate is within the bounds of this map
     * @param coord The coordinate to check
     * @return True if the coordinate is valid for this map, false otherwise
     *
     * This utility method can be used to validate coordinates before
     * attempting to access cells.
     */
    bool isValidCoordinate(const Coordinate& coord) const;

  private:
    /**
     * @brief Initializes the octagonal map with all cells
     *
     * Creates all cells in the square grid and then links them to their
     * neighbors. This is called automatically by the constructor.
     */
    void initializeMap();

    /**
     * @brief Links a cell with its neighbors in the octagonal grid
     * @param cell The cell to link with its neighbors
     *
     * Sets up bidirectional weak_ptr references between the cell and all
     * of its valid neighbors in the 8 octagonal directions.
     */
    void linkCellNeighbors(const std::shared_ptr<GameCell>& cell);

    /**
     * @brief Calculates the coordinate of a neighbor in a given direction
     * @param x The x-coordinate of the current cell
     * @param y The y-coordinate of the current cell
     * @param dir The direction to the neighbor
     * @return The coordinate of the neighbor in that direction
     *
     * This method handles the octagonal grid coordinate system where
     * each cell has up to 8 neighbors (including diagonals).
     */
    Coordinate getNeighborCoordinate(int x, int y, Direction dir) const;

    /// The radius/size parameter used to create this map
    int mapSize_;

    /// Hash map storing all cells indexed by their coordinates
    std::unordered_map<Coordinate, std::shared_ptr<GameCell>, CoordinateHash> cells_;
};

#endif  // OCTA_CORE_GRAPHGAMEMAP_H