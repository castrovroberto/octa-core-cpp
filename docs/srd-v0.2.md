Of course. Based on the excellent analysis and our previous discussion, here is a revised and iterated version of the Game Mechanics and Requirements Document. This version incorporates the clarifications and addresses the identified gaps, providing a more robust specification for the porting team.

---

## **Octa-Core C++ Port: Game Mechanics & Requirements (v2.0)**

### 1. Introduction

**Project Goal:** To port the existing Java `octa-core` game logic library to C++ as a modular, scalable, and performant library suitable for integration with game engines like Godot and Unity.

This document (v2.0) is a revision of the original specification, updated to incorporate feedback from the initial SRD analysis. It serves as the definitive guide for the C++ implementation team.

### 2. Core Concepts & Glossary

* **Cell**: The fundamental unit of the game board. Each cell has a `State` and an `Arrow Direction`.
* **State**: Defines the ownership of a cell. See `CellState` definition below.
* **Arrow Direction**: The orientation of a cell's pointer. In the octagonal grid, there are 8 directions.
* **Chain Reaction**: The core mechanic where a single move triggers a cascading capture of adjacent cells.
* **Turn**: A single player's action cycle. The turn count increments after each player's move.

### 3. System Architecture & Data Models

#### 3.1. Map Abstraction (Required)

The map's data structure **must** be abstracted behind an interface to ensure future flexibility.

* **`IGameMap` Interface**: An abstract C++ base class that defines the public contract for all map implementations.
* **Required Interface Methods**:
    * `std::shared_ptr<GameCell> getCell(const Coordinate& coord) const`
    * `std::vector<std::shared_ptr<GameCell>> getAllCells() const`
    * `int getSize() const`
    * `int getTotalCellCount() const`

#### 3.2. Concrete Map Implementation: `GraphGameMap`

The initial C++ implementation shall be a graph-based map.

* **Underlying Structure**: A `std::unordered_map<Coordinate, std::shared_ptr<GameCell>, CoordinateHash>` is required for storing cells, enabling efficient coordinate-based lookups.
* **Initialization**: The map is generated as a square grid. The `size` parameter defines the bounds from `-size` to `+size` on both X and Y axes. The total number of cells is `(2 * size + 1)²`. For example, a `size` of 2 results in a 5x5 grid, yielding 25 cells.
* **Neighbor Linking**: After creation, every cell must be linked to its 8 neighbors (N, NE, E, SE, S, SW, W, NW). This linking must be bidirectional.

#### 3.3. Core Data Models

* **`Coordinate`**: A struct/class holding `int x` and `int y` values. It must be hashable for use in `std::unordered_map`.
* **`GameCell`**: A class containing:
    * Its `Coordinate`.
    * Its `CellState`.
    * Its `Direction` (the arrow).
    * An array or map of 8 `std::shared_ptr`s to its neighboring `GameCell`s.
* **`Player`**: An `enum class` for `PLAYER_1` and `PLAYER_2`.
* **`CellState`**: An `enum class` defining the state of a cell: `NEUTRAL`, `PLAYER_1`, `PLAYER_2`, `BLOCKED`. This is distinct from the `Player` enum.
* **`Direction`**: An `enum class` for the 8 directions. A helper function, `rotateClockwise(Direction)`, must be provided to rotate a direction by 45 degrees.

### 4. Game Logic (`IGameLogic` implementation)

#### 4.1. `IGameLogic` Interface

A concrete `OctaGameLogic` class must implement the following `IGameLogic` interface:
```cpp
class IGameLogic {
public:
    virtual ~IGameLogic() = default;
    virtual bool isValidMove(std::shared_ptr<GameCell> cell, Player player) const = 0;
    virtual GameResult makeMove(std::shared_ptr<GameCell> cell, Player player) = 0;
    virtual bool isGameOver() const = 0;
    virtual Player getCurrentPlayer() const = 0;
    virtual void switchPlayer() = 0;
    virtual int getTurnCount() const = 0;
};
```

#### 4.2. State Management

The concrete `OctaGameLogic` class must manage the following internal state:
* `Player currentPlayer`
* `int turnCount`
* `GameConfig config`
* `std::optional<GameResult> gameResult`

#### 4.3. Move Validation (`isValidMove`)

* A move is valid if the selected cell is owned by the current player (`cell->getState() == player.getCellState()`).

#### 4.4. Move Execution and Chain Reaction

The `makeMove` method triggers the core game mechanic. It must be implemented as a single, non-recursive loop to prevent stack overflow issues.

**Execution Steps:**
1.  **Validate Move**: Check if the move is valid using `isValidMove`.
2.  **Initial Rotation**: Rotate the selected cell's arrow clockwise by 45 degrees.
3.  **Initiate Chain**:
    * Create a temporary `std::unordered_set<GameCell*>` named `capturedThisTurn` to track cells captured in the current move to prevent loops. Add the initial cell to this set.
    * Get the first neighbor in the chain based on the initial cell's new arrow direction.
4.  **Propagation Loop**: Loop through the chain reaction until a stopping condition is met. In each iteration:
    1.  **Check Stopping Conditions (in this order of precedence)**:
        a.  The next cell is off the grid (`nullptr`).
        b.  The next cell is already owned by the **current player**.
        c.  The next cell is already in the `capturedThisTurn` set.
        d.  The `stopOnEnemy` rule is `true` in `GameConfig` and the next cell is owned by the opponent.
        If any condition is met, terminate the loop.
    2.  **Process Cell**: If no stopping conditions are met:
        a.  Capture the cell (set its `CellState` to the current player's state).
        b.  Add the cell to the `capturedThisTurn` set.
        c.  Rotate the newly captured cell's arrow clockwise.
        d.  Set the next cell for the loop to be the neighbor indicated by the new arrow direction.
5.  **Increment Turn**: Increment the internal `turnCount` by one.
6.  **Return Result**: Return the current `GameResult`.

### 5. Game Engine & Flow

#### 5.1. `GameEngine` Responsibilities

* The engine is responsible for orchestrating the main game loop.
* It must hold `std::shared_ptr`s to the active `IGameMap`, `IGameLogic`, and `MoveProvider` instances.
* The main loop (`run` method) must perform the following sequence until `isGameOver()` is true:
    1.  Display the board.
    2.  Get a move from the `MoveProvider`.
    3.  Process the move with `gameLogic->makeMove()`.
    4.  Switch players with `gameLogic->switchPlayer()`.

#### 5.2. Input Abstraction (`MoveProvider`)

* To support both human input and automated testing, a `MoveProvider` abstract base class is required.
* The interface is `virtual std::optional<Coordinate> getNextMove() = 0;`.
* A `CLIView` class will implement this interface to get moves from the console. For testing, a mock implementation can provide a scripted queue of moves.

### 6. Win Conditions & Configuration

#### 6.1. Configuration and Results

* **`GameConfig`**: A struct to hold settings for a game session. It must contain `WinCondition winCondition`, `int turnLimit`, and `bool stopOnEnemy`.
* **`GameResult`**: A struct to encapsulate the game's outcome. It must contain `std::optional<Player> winner` (to handle ties when `std::nullopt`) and `std::string reason`.
* **`WinCondition`**: An `enum class` for `ELIMINATION` and `TURN_LIMIT_MAJORITY`.

#### 6.2. Win Condition Logic

The `isGameOver()` method in `OctaGameLogic` must implement the following checks based on the `GameConfig`:

* **`ELIMINATION`**: The game ends if one player has zero cells. This check is only active if `turnCount > 0` to prevent an immediate win on a pre-configured board.
* **`TURN_LIMIT_MAJORITY`**: The game ends when `turnCount >= turnLimit`. The player with the most cells wins. If cell counts are equal, it is a tie.

### 7. Testing Requirements

* **Unit Tests**: Must cover `GameMap` generation (cell counts, neighbor linking) and all `GameLogic` stopping conditions individually.
* **Integration Tests**: Full "play-through" tests using a scripted `MoveProvider` are required to validate complete game flows for each `WinCondition`.