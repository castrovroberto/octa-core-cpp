#include <iostream>
#include "octa-core/engine/GameEngine.h"
#include "octa-core/map/GraphGameMap.h"
#include "octa-core/map/ArrayGameMap.h"

int main() {
    std::cout << "Welcome to OctaCore C++ Edition!" << std::endl;

    try {
        // Demonstrate the modular design by using the graph-based map
        std::cout << "\n--- Creating Graph-Based Map ---" << std::endl;
        auto graphMap = std::make_shared<GraphGameMap>(4);
        GameEngine engineWithGraph(graphMap);
        engineWithGraph.run();

        // To switch to a future array-based map, you'd just change this one line:
        // (Currently throws "not implemented" exceptions, but demonstrates the modularity)
        std::cout << "\n--- Demonstrating Map Modularity (Array Map) ---" << std::endl;
        std::cout << "Creating ArrayGameMap (placeholder implementation)..." << std::endl;
        
        try {
            auto arrayMap = std::make_shared<ArrayGameMap>(4);
            std::cout << "ArrayGameMap size: " << arrayMap->getSize() << std::endl;
            std::cout << "Note: ArrayGameMap is a placeholder - methods throw 'not implemented' exceptions" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "ArrayGameMap method called: " << e.what() << std::endl;
            std::cout << "This demonstrates the interface works - just need to implement the methods!" << std::endl;
        }

        std::cout << "\n--- Modular Design Success! ---" << std::endl;
        std::cout << "The GameEngine successfully used the IGameMap interface!" << std::endl;
        std::cout << "Switching map implementations is as simple as changing one line." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
