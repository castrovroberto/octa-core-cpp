/**
 * @file GameEngine.cpp
 * @brief Implementation of the GameEngine class
 *
 * This file implements the GameEngine class methods, providing the main
 * game loop orchestration and component coordination. The engine uses
 * the IGameMap interface to remain independent of the underlying map
 * implementation.
 *
 * Updated in Phase P1.2 to use the new SRD v3.2 IGameMap interface.
 */

#include "octa-core/engine/GameEngine.h"

#include <stdexcept>

GameEngine::GameEngine(std::shared_ptr<IGameMap> map) : gameMap_(map) {
    if (!gameMap_) {
        throw std::invalid_argument("GameEngine requires a valid IGameMap implementation");
    }
}

void GameEngine::run() {
    std::cout << "GameEngine is running..." << std::endl;

    // Display map information
    displayMapInfo();

    // Demonstrate that we can access the map through the interface
    auto centerCell = gameMap_->at(Coordinate(0, 0));
    if (centerCell) {
        std::cout << "Center cell found at (" << centerCell->getCoordinate().getX() << ", "
                  << centerCell->getCoordinate().getY() << ")" << std::endl;

        std::cout << "Center cell state: ";
        switch (centerCell->getState()) {
            case CellState::NEUTRAL:
                std::cout << "NEUTRAL";
                break;
            case CellState::PLAYER_1:
                std::cout << "PLAYER_1";
                break;
            case CellState::PLAYER_2:
                std::cout << "PLAYER_2";
                break;
            case CellState::BLOCKED:
                std::cout << "BLOCKED";
                break;
        }
        std::cout << std::endl;

        // Show neighbor count using the new API
        std::cout << "Center cell has " << centerCell->getValidNeighborCount() << " neighbors"
                  << std::endl;
    }

    // The main 'while (!logic->isGameOver())' loop would go here.
    std::cout << "Game loop would continue here..." << std::endl;
}

void GameEngine::displayMapInfo() const {
    std::cout << "\n=== Map Information ===" << std::endl;
    std::cout << "Total cells: " << gameMap_->size() << std::endl;

    // Sample a few cells to show the map is working
    std::cout << "\nSample cells:" << std::endl;
    std::vector<Coordinate> sampleCoords = {
        Coordinate(0, 0),   // Center
        Coordinate(1, 0),   // East
        Coordinate(0, 1),   // North
        Coordinate(-1, 1),  // North-West
        Coordinate(2, 2),   // Far corner (may not exist in smaller maps)
    };

    for (const auto& coord : sampleCoords) {
        auto cell = gameMap_->at(coord);
        std::cout << "  Cell at (" << coord.getX() << ", " << coord.getY()
                  << "): " << (cell ? "EXISTS" : "NULL") << std::endl;
    }
    std::cout << "======================\n" << std::endl;
}