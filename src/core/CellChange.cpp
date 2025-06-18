/**
 * @file CellChange.cpp
 * @brief Implementation of CellChange struct methods
 */

#include "octa-core/core/CellChange.h"
#include "octa-core/core/GameCell.h"
#include <stdexcept>

void CellChange::restore() const {
    if (!isValid()) {
        throw std::runtime_error("CellChange: cannot restore - invalid cell pointer");
    }

    cell->setState(oldState);
    cell->setDirection(oldDirection);
} 