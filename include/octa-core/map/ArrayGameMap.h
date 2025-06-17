#ifndef OCTA_CORE_ARRAYGAMEMAP_H
#define OCTA_CORE_ARRAYGAMEMAP_H

#include "IGameMap.h"

/**
 * @class ArrayGameMap
 * @brief A future implementation of IGameMap using a 2D array (vector of vectors).
 *
 * This class would be efficient for dense, square grids. Currently serves as
 * a placeholder to demonstrate the modular design's extensibility.
 */
class ArrayGameMap : public IGameMap {
public:
    /**
     * @brief Constructor that creates a map of the specified size.
     * @param size The size of the map.
     */
    explicit ArrayGameMap(int size);

    // IGameMap interface implementation
    std::shared_ptr<GameCell> getCell(const Coordinate& coord) const override;
    std::vector<std::shared_ptr<GameCell>> getAllCells() const override;
    int getSize() const override;
    int getTotalCellCount() const override;

private:
    /**
     * @brief Converts world coordinates to array indices.
     * @param coord The world coordinate to convert.
     * @return The converted coordinate for array indexing.
     */
    Coordinate worldToArray(const Coordinate& coord) const;

    /**
     * @brief Checks if array indices are valid.
     * @param arrayCoord The array coordinate to validate.
     * @return True if the coordinate is valid, false otherwise.
     */
    bool isValidArrayCoordinate(const Coordinate& arrayCoord) const;

    int mapSize_;
    // The underlying data structure is now a 2D vector
    std::vector<std::vector<std::shared_ptr<GameCell>>> grid_;
};

#endif // OCTA_CORE_ARRAYGAMEMAP_H 