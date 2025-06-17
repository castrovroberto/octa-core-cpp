#include "octa-core/map/GraphGameMap.h"
#include <cmath>
#include <algorithm>

GraphGameMap::GraphGameMap(int size) : mapSize_(size) {
    initializeMap();
}

std::shared_ptr<GameCell> GraphGameMap::getCell(const Coordinate& coord) const {
    auto it = cells_.find(coord);
    return (it != cells_.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<GameCell>> GraphGameMap::getAllCells() const {
    std::vector<std::shared_ptr<GameCell>> allCells;
    allCells.reserve(cells_.size());
    
    for (const auto& pair : cells_) {
        allCells.push_back(pair.second);
    }
    
    return allCells;
}

int GraphGameMap::getSize() const {
    return mapSize_;
}

int GraphGameMap::getTotalCellCount() const {
    return static_cast<int>(cells_.size());
}

void GraphGameMap::initializeMap() {
    // Create all cells first
    for (int x = -mapSize_; x <= mapSize_; ++x) {
        for (int y = -mapSize_; y <= mapSize_; ++y) {
            Coordinate coord(x, y);
            if (isValidCoordinate(coord)) {
                cells_[coord] = std::make_shared<GameCell>(coord);
            }
        }
    }
    
    // Then link neighbors
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
            cell->setNeighbor(direction, neighborIt->second);
        }
    }
}

Coordinate GraphGameMap::getNeighborCoordinate(int x, int y, Direction dir) const {
    // Octagonal grid neighbor calculations
    // Using standard 8-directional grid with cardinal and diagonal directions
    switch (dir) {
        case Direction::NORTH:
            return Coordinate(x, y + 1);
        case Direction::NORTH_EAST:
            return Coordinate(x + 1, y + 1);
        case Direction::EAST:
            return Coordinate(x + 1, y);
        case Direction::SOUTH_EAST:
            return Coordinate(x + 1, y - 1);
        case Direction::SOUTH:
            return Coordinate(x, y - 1);
        case Direction::SOUTH_WEST:
            return Coordinate(x - 1, y - 1);
        case Direction::WEST:
            return Coordinate(x - 1, y);
        case Direction::NORTH_WEST:
            return Coordinate(x - 1, y + 1);
        default:
            return Coordinate(x, y); // Should never happen
    }
}

bool GraphGameMap::isValidCoordinate(const Coordinate& coord) const {
    int x = coord.getX();
    int y = coord.getY();
    
    // For octagonal grid, use Chebyshev distance (max of |x|, |y|) <= mapSize
    // This creates a square boundary which is appropriate for 8-directional movement
    return (std::abs(x) <= mapSize_ && std::abs(y) <= mapSize_);
} 