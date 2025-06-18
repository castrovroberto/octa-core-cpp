/**
 * @file CellChange.h
 * @brief Cell modification tracking for light undo system
 *
 * This file defines the CellChange struct used to record cell state
 * changes for transactional rollback in the LIGHT_UNDO safety system.
 *
 * Created for Phase P2.1 according to SRD v3.2 specifications.
 */

#ifndef OCTA_CORE_CELLCHANGE_H
#define OCTA_CORE_CELLCHANGE_H

#include "Direction.h"
#include "Enums.h"

#include <memory>

// Forward declaration to avoid circular dependency with GameCell.h
class GameCell;

/**
 * @struct CellChange
 * @brief Records the state of a GameCell before modification for rollback purposes
 *
 * This lightweight structure captures the essential state of a cell before
 * it is modified during a move operation. It enables transactional rollback
 * in the LIGHT_UNDO safety system by storing:
 * - A shared pointer to the modified cell
 * - The cell's state before modification
 * - The cell's direction before modification
 *
 * Key design decisions:
 * - Uses shared_ptr to ensure cell validity during rollback
 * - Stores both state and direction for complete restoration
 * - Lightweight struct for minimal performance overhead
 * - Immutable after construction to prevent accidental modification
 */
struct CellChange {
    std::shared_ptr<GameCell> cell;  ///< Pointer to the modified cell
    CellState oldState;              ///< Cell state before modification
    Direction oldDirection;          ///< Cell direction before modification

    /**
     * @brief Constructs a CellChange record
     * @param c Shared pointer to the cell that was modified
     * @param os The cell's state before modification
     * @param od The cell's direction before modification
     *
     * @throws std::invalid_argument if cell pointer is null
     */
    CellChange(std::shared_ptr<GameCell> c, CellState os, Direction od)
        : cell(c), oldState(os), oldDirection(od) {
        if (!cell) {
            throw std::invalid_argument("CellChange: cell pointer cannot be null");
        }
    }

    /**
     * @brief Checks if the cell pointer is still valid
     * @return True if the cell pointer is valid, false otherwise
     */
    bool isValid() const { return cell != nullptr; }

    /**
     * @brief Restores the cell to its recorded state
     *
     * This method applies the recorded state and direction back to the cell.
     * It should only be called during rollback operations.
     *
     * @throws std::runtime_error if the cell pointer is invalid
     */
    void restore() const {
        if (!isValid()) {
            throw std::runtime_error("CellChange: cannot restore - invalid cell pointer");
        }

        cell->setState(oldState);
        cell->setDirection(oldDirection);
    }
};

#endif  // OCTA_CORE_CELLCHANGE_H