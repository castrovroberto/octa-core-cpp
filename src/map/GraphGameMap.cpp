/**
 * @file GraphGameMap.cpp
 * @brief Implementation of the GraphGameMap class
 *
 * This file implements the GraphGameMap class methods, providing a hash map-based
 * implementation of the IGameMap interface. The implementation handles octagonal
 * grid initialization, neighbor linking, and efficient coordinate-based lookups.
 *
 * Updated in Phase P1.2 to implement the new SRD v3.2 interface specification
 * and work with the refactored GameCell class.
 */

#include "octa-core/map/GraphGameMap.h"

#include <algorithm>
#include <cmath>

GraphGameMap::GraphGameMap(int size) : mapSize_(size) {
    initializeMap();
}

std::shared_ptr<GameCell> GraphGameMap::at(const Coordinate& coord) const {
    auto it = cells_.find(coord);
    return (it != cells_.end()) ? it->second : nullptr;
}

size_t GraphGameMap::size() const {
    return cells_.size();
}

bool GraphGameMap::isValidCoordinate(const Coordinate& coord) const {
    int x = coord.getX();
    int y = coord.getY();

    // For octagonal grid, use Chebyshev distance (max of |x|, |y|) <= mapSize
    // This creates a square boundary which is appropriate for 8-directional movement
    return (std::abs(x) <= mapSize_ && std::abs(y) <= mapSize_);
}

void GraphGameMap::initializeMap() {
    // Create all cells first with NEUTRAL state and default direction
    for (int x = -mapSize_; x <= mapSize_; ++x) {
        for (int y = -mapSize_; y <= mapSize_; ++y) {
            Coordinate coord(x, y);
            if (isValidCoordinate(coord)) {
                // Initialize cells with NEUTRAL state and N direction
                cells_[coord] = std::make_shared<GameCell>(coord, CellState::NEUTRAL);
            }
        }
    }

    // Then link neighbors using weak_ptr references
    for (const auto& pair : cells_) {
        linkCellNeighbors(pair.second);
    }
}

void GraphGameMap::linkCellNeighbors(const std::shared_ptr<GameCell>& cell) {
    const Coordinate& coord = cell->getCoordinate();
    int x = coord.getX();
    int y = coord.getY();

    // Link all 8 octagonal directions
    for (int dir = 0; dir < 8; ++dir) {
        Direction direction = static_cast<Direction>(dir);
        Coordinate neighborCoord = getNeighborCoordinate(x, y, direction);

        auto neighborIt = cells_.find(neighborCoord);
        if (neighborIt != cells_.end()) {
            // Set neighbor using the new GameCell API
            cell->setNeighbor(direction, neighborIt->second);
        }
    }
}

Coordinate GraphGameMap::getNeighborCoordinate(int x, int y, Direction dir) const {
    // Octagonal grid neighbor calculations
    // Using standard 8-directional grid with cardinal and diagonal directions
    switch (dir) {
        case Direction::N:
            return Coordinate(x, y + 1);
        case Direction::NE:
            return Coordinate(x + 1, y + 1);
        case Direction::E:
            return Coordinate(x + 1, y);
        case Direction::SE:
            return Coordinate(x + 1, y - 1);
        case Direction::S:
            return Coordinate(x, y - 1);
        case Direction::SW:
            return Coordinate(x - 1, y - 1);
        case Direction::W:
            return Coordinate(x - 1, y);
        case Direction::NW:
            return Coordinate(x - 1, y + 1);
        default:
            return Coordinate(x, y);  // Should never happen
    }
}