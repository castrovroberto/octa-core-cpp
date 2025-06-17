Of course. Here is the comprehensive "Final Requirements Specification" document. This consolidated version (v3.1) integrates all the iterative refinements and decisions from our discussions, providing a complete and actionable guide for the development team.

---

## **Octa-Core C++ Port: Final Requirements Specification (v3.1)**

### 1. Introduction

**Project Goal:** To port the existing Java `octa-core` game logic library to C++ as a modular, scalable, and performant library suitable for integration with game engines like Godot and Unity.

This document is the definitive specification for the C++ implementation. It supersedes all previous versions and incorporates feedback from all stages of the design and review process.

### 2. Core Concepts & Glossary

* **Cell**: The fundamental unit of the game board. Each cell has a `State` and an `Arrow Direction`.
* **State**: Defines the ownership of a cell. See `CellState` definition below.
* **Arrow Direction**: The orientation of a cell's pointer. In the octagonal grid, there are 8 directions.
* **Chain Reaction**: The core mechanic where a single move triggers a cascading capture of adjacent cells.
* **Turn**: A single player's action cycle. The turn count increments after each player's move.

### 3. System Architecture & Data Models

#### 3.1. Map Abstraction (`IGameMap`)

The map's data structure **must** be abstracted behind an interface to ensure future flexibility.

* **`IGameMap` Interface**: An abstract C++ base class defining the public contract for all map implementations.
* **Concrete `GraphGameMap`**: The initial implementation shall use a `std::unordered_map<Coordinate, std::shared_ptr<GameCell>, CoordinateHash>` for cell storage.
* **Initialization**: The map is a square grid where `size` defines the bounds from `-size` to `+size` on both axes. The total cell count is `(2 * size + 1)²`. A `size` of 2 results in a 5x5 (25 cell) grid.
* **Neighbor Linking**: All 8 neighbors for each cell must be linked bidirectionally. See Section 8 for memory management specifics (`weak_ptr`).

#### 3.2. Core Data Models

* **`GameCell`**: The class must contain the following private members: `Coordinate coordinate_`, `CellState state_`, and `Direction direction_`. The `Player occupant_` concept from earlier drafts is superseded by `CellState`.
* **`Direction`**: An `enum class` for the 8 directions. It must be accompanied by `inline` helper functions `rotateClockwise(Direction)` and `rotateCounterClockwise(Direction)`.
* **`CellState`**: An `enum class` defining cell state: `NEUTRAL`, `PLAYER_1`, `PLAYER_2`, `BLOCKED`.
* **`Player`**: An `enum class` for `PLAYER_1` and `PLAYER_2`.

### 4. Game Logic (`OctaGameLogic`)

#### 4.1. `IGameLogic` Interface

The `OctaGameLogic` class must implement the following public interface:

```cpp
class IGameLogic {
public:
    virtual ~IGameLogic() = default;
    virtual bool isValidMove(std::shared_ptr<const GameCell> cell, Player player) const = 0;
    virtual GameResult makeMove(std::shared_ptr<GameCell> cell, Player player) = 0;
    virtual bool isGameOver() const = 0;
    virtual Player getCurrentPlayer() const = 0;
    virtual void switchPlayer() = 0;
    virtual int getTurnCount() const = 0;
    virtual const GameConfig& getConfig() const = 0;
};
```

#### 4.2. State Management

`OctaGameLogic` is the single source of truth for game state and must own the following private members: `Player currentPlayer`, `int turnCount`, `GameConfig config`, and `std::optional<GameResult> gameResult`.

#### 4.3. Chain Reaction Algorithm (`makeMove`)

The `makeMove` method implements the core game mechanic and must adhere to the strong exception safety guarantee (see Section 7).

**Execution Steps:**
1.  **Validate Move**: Throw `InvalidMoveException` if `isValidMove` returns `false`.
2.  **Setup Rollback**: Begin the transaction for exception safety (see Section 7.2).
3.  **Initial Rotation**: Rotate the selected cell's arrow clockwise. Record the undo command.
4.  **Initiate Chain**:
    * Create a temporary `std::unordered_set<GameCell*>` named `capturedThisTurn`. Add the initial cell.
    * Get the first neighbor in the chain based on the new arrow direction.
5.  **Propagation Loop**: Loop through the chain reaction until a stopping condition is met. In each iteration:
    1.  **Check Stopping Conditions** (in order of precedence):
        a. The next cell is off-grid (`nullptr`).
        b. The next cell is already owned by the **current player**.
        c. The next cell is already in the `capturedThisTurn` set (loop prevention).
        d. The `stopOnEnemy` rule is `true` and the next cell is owned by the opponent.
        If any condition is met, terminate the loop.
    2.  **Process Cell**: If no stopping conditions are met:
        a. Record the cell's current state in the undo list.
        b. Capture the cell (set its `CellState`).
        c. Rotate the newly captured cell's arrow.
        d. Add the cell to `capturedThisTurn`.
        e. Set the next cell for the loop.
6.  **Finalize Turn**: Increment `turnCount` and return the `GameResult`.

### 5. Game Engine & Flow

#### 5.1. `GameEngine` Responsibilities

The engine orchestrates the game loop, holding `std::shared_ptr`s to the `IGameMap`, `IGameLogic`, and `MoveProvider`.

#### 5.2. `MoveProvider` Interface

To decouple input sources, the `MoveProvider` interface must be implemented as follows, providing necessary game state context:

```cpp
class MoveProvider {
public:
    virtual ~MoveProvider() = default;
    virtual std::optional<Coordinate> getNextMove(const IGameMap& map, Player currentPlayer) = 0;
};
```

### 6. Configuration & Win Conditions

* **`GameConfig`**: A struct holding `WinCondition winCondition`, `int turnLimit`, and `bool stopOnEnemy`.
* **`GameResult`**: A struct holding `std::optional<Player> winner` and `std::string reason`.
* **Win Conditions**:
    * **`ELIMINATION`**: Ends when a player has zero cells (checked only if `turnCount > 0`).
    * **`TURN_LIMIT_MAJORITY`**: Ends when `turnCount >= turnLimit`. The player with more cells wins. A tie is possible (`winner` is `std::nullopt`).

### 7. Error Handling & Exception Safety

* **Strategy**: Invalid operations **must** be reported by throwing exceptions from a custom hierarchy (e.g., `InvalidMoveException` inheriting from `OctaCoreException`).
* **Strong Exception Safety**: `makeMove` **must** be transactional. If it fails for any reason, the game state must be rolled back to its condition before the call.
* **Rollback Mechanism**: The **Command Pattern** is the required implementation for rollback. An "undo list" of cell states must be collected before modifications and used to restore state in a `catch` block.

### 8. Memory Management Policy

* **`GameCell` Ownership**: The `IGameMap` holds the primary owning pointers as `std::shared_ptr<GameCell>`.
* **Neighbor Pointers**: To prevent circular references, a `GameCell`'s pointers to its neighbors **must** be stored as `std::weak_ptr<GameCell>`. Accessing a neighbor requires locking the `weak_ptr`.
* **Value Types**: `Coordinate`, `GameConfig`, and `GameResult` should be treated as value types (passed by value or const reference).

### 9. Development Process & Tooling

#### 9.1. Testing

* **Framework**: **Google Test** is the mandated testing framework.
* **Coverage**: Core logic must meet a minimum of **85%** test coverage.
* **Integration Scenarios**: Tests for each win condition, invalid move handling, and chain reaction edge cases are required.
* **Memory & Performance**: The test suite must include steps for memory leak detection (e.g., via **Valgrind**) and performance benchmarking against specified targets.

#### 9.2. Build System & Standards

* **Build System**: **CMake** is required. The build must produce the main game executable and a separate test runner executable.
* **Code Style**: The **Google C++ Style Guide** is the required standard.
* **Documentation**: Public APIs **must** be documented using **Doxygen-style** comments.