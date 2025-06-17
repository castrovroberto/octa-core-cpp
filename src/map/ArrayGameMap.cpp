/**
 * @file ArrayGameMap.cpp
 * @brief Implementation of the ArrayGameMap class placeholder
 * 
 * This file provides placeholder implementations for the ArrayGameMap class.
 * All methods throw runtime_error with descriptive messages indicating that
 * this implementation is not yet available.
 * 
 * Updated in Phase P1.2 to implement the new SRD v3.2 interface specification.
 */

#include "octa-core/map/ArrayGameMap.h"
#include <stdexcept>

ArrayGameMap::ArrayGameMap(int size) : mapSize_(size) {
    throw std::runtime_error("ArrayGameMap constructor not implemented yet - use GraphGameMap instead");
}

std::shared_ptr<GameCell> ArrayGameMap::at(const Coordinate& coord) const {
    (void)coord; // Suppress unused parameter warning
    throw std::runtime_error("ArrayGameMap::at() not implemented yet - use GraphGameMap instead");
}

size_t ArrayGameMap::size() const {
    throw std::runtime_error("ArrayGameMap::size() not implemented yet - use GraphGameMap instead");
}

Coordinate ArrayGameMap::worldToArray(const Coordinate& coord) const {
    (void)coord; // Suppress unused parameter warning
    throw std::runtime_error("ArrayGameMap::worldToArray() not implemented yet");
}

bool ArrayGameMap::isValidArrayCoordinate(const Coordinate& arrayCoord) const {
    (void)arrayCoord; // Suppress unused parameter warning
    throw std::runtime_error("ArrayGameMap::isValidArrayCoordinate() not implemented yet");
} 