### Part 1: The `IGameMap` Interface

First, we define the abstract base class. This is the contract that all future map implementations must follow. It contains only the essential operations the game needs.

Create a new header file for the interface:

**`octa-core-cpp/include/octa-core/map/IGameMap.h`**
```cpp
#ifndef OCTA_CORE_IGAMEMAP_H
#define OCTA_CORE_IGAMEMAP_H

#include <vector>
#include <memory>
#include "../model/Coordinate.h"
#include "../core/GameCell.h"

// Forward declaration
class GameCell;

/**
 * @class IGameMap
 * @brief An interface for the game map, abstracting the underlying data structure.
 *
 * This abstract base class defines the essential operations for a game map,
 * allowing the game engine to function independently of whether the map is
 * stored as a graph, an array, or any other structure.
 */
class IGameMap {
public:
    // Virtual destructor is essential for a polymorphic base class.
    virtual ~IGameMap() = default;

    /**
     * @brief Retrieves a cell at a specific coordinate.
     * @param coord The coordinate of the cell to retrieve.
     * @return A shared pointer to the GameCell, or nullptr if no cell exists.
     */
    virtual std::shared_ptr<GameCell> getCell(const Coordinate& coord) const = 0;

    /**
     * @brief Retrieves a list of all cells in the map.
     * @return A vector of shared pointers to all GameCells.
     */
    virtual std::vector<std::shared_ptr<GameCell>> getAllCells() const = 0;

    /**
     * @brief Gets the size (radius) of the map.
     * @return The integer size of the map.
     */
    virtual int getSize() const = 0;

    /**
     * @brief Gets the total number of cells in the map.
     * @return The total cell count.
     */
    virtual int getTotalCellCount() const = 0;
};

#endif // OCTA_CORE_IGAMEMAP_H
```
* **Pure Virtual Functions (`= 0`)**: These functions *must* be implemented by any class that inherits from `IGameMap`.
* **`virtual ~IGameMap()`**: A virtual destructor is critical for any class intended to be used as a base class.

---

### Part 2: The Graph-Based Implementation

Now, let's create your desired graph-based map. It will implement the `IGameMap` interface. The underlying structure will be a hash map, which is excellent for sparse grids and graph-like structures where lookup by coordinate is key.

**`octa-core-cpp/include/octa-core/map/GraphGameMap.h`**
```cpp
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

class GraphGameMap : public IGameMap {
public:
    explicit GraphGameMap(int size);

    std::shared_ptr<GameCell> getCell(const Coordinate& coord) const override;
    std::vector<std::shared_ptr<GameCell>> getAllCells() const override;
    int getSize() const override;
    int getTotalCellCount() const override;

private:
    void initializeMap();
    void linkCellNeighbors(const std::shared_ptr<GameCell>& cell);
    Coordinate getNeighborCoordinate(int x, int y, Direction dir) const;

    int mapSize;
    std::unordered_map<Coordinate, std::shared_ptr<GameCell>, CoordinateHash> cells;
};

#endif // OCTA_CORE_GRAPHGAMEMAP_H
```

You would then implement the functions in `GraphGameMap.cpp`. The logic would be very similar to the `GameMap.java` you had previously.

---

### Part 3: The (Future) Array-Based Implementation

To show the power of this design, here is the header for a future array-based implementation. Notice it also inherits from `IGameMap`, but its private members are completely different.

**`octa-core-cpp/include/octa-core/map/ArrayGameMap.h`**
```cpp
#ifndef OCTA_CORE_ARRAYGAMEMAP_H
#define OCTA_CORE_ARRAYGAMEMAP_H

#include "IGameMap.h"

/**
 * @class ArrayGameMap
 * @brief A future implementation of IGameMap using a 2D array (vector of vectors).
 *
 * This class would be efficient for dense, square grids.
 */
class ArrayGameMap : public IGameMap {
public:
    explicit ArrayGameMap(int size);

    std::shared_ptr<GameCell> getCell(const Coordinate& coord) const override;
    std::vector<std::shared_ptr<GameCell>> getAllCells() const override;
    int getSize() const override;
    int getTotalCellCount() const override;

private:
    int mapSize;
    // The underlying data structure is now a 2D vector
    std::vector<std::vector<std::shared_ptr<GameCell>>> grid;
    // Helper to map (x,y) coordinates to array indices
    Coordinate worldToArray(const Coordinate& coord) const;
};

#endif // OCTA_CORE_ARRAYGAMEMAP_H
```

---

### Part 4: Putting It All Together

Now, your `GameEngine` will hold a pointer to the `IGameMap` interface, not a concrete class.

**`octa-core-cpp/include/octa-core/engine/GameEngine.h` (Revised)**
```cpp
#include "../map/IGameMap.h" // Include the interface
#include <memory>

class GameEngine {
public:
    // The engine accepts any object that implements IGameMap
    GameEngine(std::shared_ptr<IGameMap> map /*, other components */)
        : gameMap(map) {}

    void run() {
        // The engine doesn't know or care how the map is stored.
        // It just uses the interface methods.
        int totalCells = gameMap->getTotalCellCount();
        std::cout << "Map has " << totalCells << " cells." << std::endl;

        auto cell = gameMap->getCell(Coordinate(0, 0));
        // ...
    }

private:
    // Store a pointer to the interface, not the concrete class!
    std::shared_ptr<IGameMap> gameMap;
    // ... other members like logic, view
};
```

In your `Main.cpp`, you can now "seamlessly" switch the map implementation:

**`octa-core-cpp/src/Main.cpp` (Revised)**
```cpp
#include "octa-core/map/GraphGameMap.h"
#include "octa-core/map/ArrayGameMap.h"
#include "octa-core/engine/GameEngine.h"

int main() {
    // To use the graph-based map:
    auto graphMap = std::make_shared<GraphGameMap>(4);
    GameEngine engineWithGraph(graphMap);
    engineWithGraph.run();

    // To switch to a future array-based map, you'd just change this one line:
    // auto arrayMap = std::make_shared<ArrayGameMap>(4);
    // GameEngine engineWithArray(arrayMap);
    // engineWithArray.run();

    return 0;
}
```