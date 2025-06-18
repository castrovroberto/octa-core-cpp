/**
 * @file Main.cpp
 * @brief Main entry point for the Octa-Core C++ game
 *
 * This file demonstrates the modular design of the game engine by showing
 * how different map implementations can be used interchangeably through
 * the IGameMap interface.
 *
 * Updated in Phase P1.2 to use the new SRD v3.2 IGameMap interface.
 */

#include "octa-core/engine/GameEngine.h"
#include "octa-core/logic/OctaGameLogic.h"
#include "octa-core/map/ArrayGameMap.h"
#include "octa-core/map/GraphGameMap.h"
#include "octa-core/model/GameConfig.h"

#include <iostream>

int main() {
    std::cout << "Welcome to OctaCore C++ Edition!" << std::endl;

    try {
        // Demonstrate the modular design by using the graph-based map
        std::cout << "\n--- Creating Graph-Based Map ---" << std::endl;
        auto graphMap = std::make_shared<GraphGameMap>(4);
        std::cout << "GraphGameMap created with " << graphMap->size() << " cells" << std::endl;

        GameEngine engineWithGraph(graphMap);
        engineWithGraph.run();

        // Demonstrate the interface modularity with ArrayGameMap placeholder
        std::cout << "\n--- Demonstrating Map Modularity (Array Map) ---" << std::endl;
        std::cout << "Attempting to create ArrayGameMap (placeholder implementation)..."
                  << std::endl;

        try {
            auto arrayMap = std::make_shared<ArrayGameMap>(4);
            // This line won't be reached due to constructor exception
            std::cout << "ArrayGameMap size: " << arrayMap->size() << std::endl;
        } catch (const std::runtime_error& e) {
            std::cout << "Expected exception: " << e.what() << std::endl;
            std::cout
                << "This demonstrates the interface works - just need to implement the methods!"
                << std::endl;
        }

        // Demonstrate P1.3 Game Logic
        std::cout << "\n--- Demonstrating Game Logic (P1.3) ---" << std::endl;
        try {
            auto gameLogic = std::make_unique<OctaGameLogic>(graphMap);
            std::cout << "OctaGameLogic created successfully!" << std::endl;
            std::cout << "Current player: Player "
                      << (static_cast<int>(gameLogic->getCurrentPlayer()) + 1) << std::endl;
            std::cout << "Turn count: " << gameLogic->getTurnCount() << std::endl;
            std::cout << "Game over: " << (gameLogic->isGameOver() ? "Yes" : "No") << std::endl;

            // Make a sample move
            auto centerCell = graphMap->at(Coordinate(0, 0));
            if (centerCell && gameLogic->isValidMove(centerCell, gameLogic->getCurrentPlayer())) {
                std::cout << "Making move at center cell..." << std::endl;
                GameResult result = gameLogic->makeMove(centerCell, gameLogic->getCurrentPlayer());
                std::cout << "Move result: " << result.reason << std::endl;
                std::cout << "Current player: Player "
                          << (static_cast<int>(gameLogic->getCurrentPlayer()) + 1) << std::endl;
                std::cout << "Turn count: " << gameLogic->getTurnCount() << std::endl;
                std::cout << "Center cell value: " << centerCell->getValue() << std::endl;
                std::cout << "Center cell state: "
                          << (centerCell->getState() == CellState::PLAYER_1 ? "PLAYER_1" : "OTHER")
                          << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Game logic error: " << e.what() << std::endl;
        }

        std::cout << "\n--- Modular Design Success! ---" << std::endl;
        std::cout << "The GameEngine successfully used the IGameMap interface!" << std::endl;
        std::cout << "Game Logic successfully demonstrated OctaCore mechanics!" << std::endl;
        std::cout << "Switching map implementations is as simple as changing one line."
                  << std::endl;
        std::cout << "Current interface methods: at() and size()" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
