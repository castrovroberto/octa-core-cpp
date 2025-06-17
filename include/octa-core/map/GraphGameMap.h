#ifndef OCTA_CORE_GRAPHGAMEMAP_H
#define OCTA_CORE_GRAPHGAMEMAP_H

#include "IGameMap.h"
#include <unordered_map>

// We need a custom hash function for Coordinate to be used in unordered_map
struct CoordinateHash {
    std::size_t operator()(const Coordinate& coord) const {
        // A simple hash combination
        auto hashX = std::hash<int>()(coord.getX());
        auto hashY = std::hash<int>()(coord.getY());
        return hashX ^ (hashY << 1);
    }
};

/**
 * @class GraphGameMap
 * @brief A graph-based implementation of IGameMap using hash maps.
 *
 * This implementation uses an unordered_map for efficient coordinate-based
 * lookups and is ideal for sparse hexagonal grids where cells are connected
 * in a graph-like structure.
 */
class GraphGameMap : public IGameMap {
public:
    /**
     * @brief Constructor that creates a hexagonal map of the specified size.
     * @param size The radius of the hexagonal map.
     */
    explicit GraphGameMap(int size);

    // IGameMap interface implementation
    std::shared_ptr<GameCell> getCell(const Coordinate& coord) const override;
    std::vector<std::shared_ptr<GameCell>> getAllCells() const override;
    int getSize() const override;
    int getTotalCellCount() const override;

private:
    /**
     * @brief Initializes the hexagonal map with all cells.
     */
    void initializeMap();

    /**
     * @brief Links a cell with its neighbors in the hexagonal grid.
     * @param cell The cell to link with its neighbors.
     */
    void linkCellNeighbors(const std::shared_ptr<GameCell>& cell);

    /**
     * @brief Calculates the coordinate of a neighbor in a given direction.
     * @param x The x-coordinate of the current cell.
     * @param y The y-coordinate of the current cell.
     * @param dir The direction to the neighbor.
     * @return The coordinate of the neighbor.
     */
    Coordinate getNeighborCoordinate(int x, int y, Direction dir) const;

    /**
     * @brief Checks if a coordinate is within the bounds of the hexagonal map.
     * @param coord The coordinate to check.
     * @return True if the coordinate is valid, false otherwise.
     */
    bool isValidCoordinate(const Coordinate& coord) const;

    int mapSize_;
    std::unordered_map<Coordinate, std::shared_ptr<GameCell>, CoordinateHash> cells_;
};

#endif // OCTA_CORE_GRAPHGAMEMAP_H 