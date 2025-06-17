Of course. Here is a detailed Game Mechanics and Requirements Document for the C++ porting team. This document synthesizes the features from the Java implementation, the project backlog, and the design plans into a single, comprehensive guide.

---

## **Octa-Core C++ Port: Game Mechanics & Requirements**

### 1. Introduction

**Project Goal:** To port the existing Java `octa-core` game logic library to C++ a modular, scalable, and performant library suitable for integration with game engines like Godot and Unity.

This document outlines the core mechanics, system architecture, and feature requirements for the C++ implementation team. The primary objective is to replicate the functionality of the Java version while adopting modern C++ best practices.

### 2. Core Concepts & Glossary

* **Cell**: The fundamental unit of the game board. Each cell has a `State` and an `Arrow Direction`.
* **State**: Defines the ownership of a cell. Can be `NEUTRAL`, `PLAYER_1`, or `PLAYER_2`.
* **Arrow Direction**: The orientation of a cell's pointer. In the octagonal grid, there are 8 directions: `NORTH`, `NORTHEAST`, `EAST`, `SOUTHEAST`, `SOUTH`, `SOUTHWEST`, `WEST`, `NORTHWEST`.
* **Chain Reaction**: The core mechanic where a single move triggers a cascading capture of adjacent cells.
* **Turn**: A single player's action cycle.

### 3. System Architecture & Data Models

#### 3.1. Map Abstraction (Required)

To ensure future flexibility, the map's data structure must be abstracted.

* **`IGameMap` Interface**: An abstract base class that defines the public contract for all map implementations. The rest of the application will interact with the map *only* through this interface.
* **Interface Methods**:
    * `getCell(const Coordinate& coord)`: Retrieves the cell at a given coordinate.
    * `getAllCells()`: Returns a list of all cells in the map.
    * `getSize()`: Returns the map's radius or size parameter.
    * `getTotalCellCount()`: Returns the total number of instantiated cells.

#### 3.2. Concrete Map Implementation: `GraphGameMap`

The initial C++ implementation shall be a graph-based map.

* **Underlying Structure**: Use a `std::unordered_map<Coordinate, std::shared_ptr<GameCell>>` to store cells. This is efficient for coordinate-based lookups.
* **Initialization**: The map is generated as a square grid of cells from `-size` to `+size` on both X and Y axes. A size of 2 results in a 5x5 grid, yielding 25 cells.
* **Neighbor Linking**: After creation, every cell must be linked to its 8 neighbors (including diagonals). A cell at `(x, y)` has its `NORTH` neighbor at `(x, y+1)`, `NORTHEAST` at `(x+1, y+1)`, and so on. This linking must be bidirectional.

#### 3.3. Core Data Models

* **`Coordinate`**: A simple struct/class holding `int x` and `int y` values. It must be hashable to be used as a key in `std::unordered_map`.
* **`GameCell`**: A class containing:
    * Its `Coordinate`.
    * Its current `CellState`.
    * Its current `Direction` (the arrow).
    * An array or map of 8 pointers to its neighboring `GameCell`s.
* **`Player`**: An `enum class` for `PLAYER_1` and `PLAYER_2`.
* **`Direction`**: An `enum class` for the 8 directions. It must include a `rotateClockwise()` helper method that increments the direction by 45 degrees (e.g., `NORTH` becomes `NORTHEAST`).

### 4. Game Logic (`IGameLogic` implementation)

This is the engine's core ruleset.

#### 4.1. Move Validation (`isValidMove`)

* A move is considered valid if and only if the selected cell is owned by the current player.
* The function signature should be `bool isValidMove(std::shared_ptr<GameCell> cell, Player player)`.

#### 4.2. Move Execution & Chain Reaction (`makeMove`)

Executing a move is a two-stage process:

1.  **Initial Rotation**: The arrow of the player's selected cell is rotated clockwise by one increment (45 degrees).
2.  **Chain Initiation**: A chain reaction is initiated, starting from the neighbor indicated by the cell's *new* arrow direction.

#### 4.3. Chain Reaction Propagation

The propagation logic must be implemented as a loop to prevent stack overflow on long chains. For each cell in the chain:

1.  **Check Stopping Conditions**: Before processing, check if the chain should terminate (see section 4.4).
2.  **Capture Cell**: The cell's state is changed to that of the current player.
3.  **Rotate Arrow**: The newly captured cell's arrow is rotated clockwise by one increment.
4.  **Propagate**: The next cell in the chain is the one indicated by the *newly rotated* arrow.

#### 4.4. Chain Reaction Stopping Conditions

A chain reaction **must** terminate if the next cell in the sequence meets any of these conditions:

* **Grid Edge**: The target coordinate is outside the map bounds (i.e., `getCell` returns `nullptr`).
* **Hits Own Cell**: The target cell is already owned by the current player. The chain stops, and the target cell is NOT modified.
* **Hits Loop**: The target cell has already been captured *during the current turn*. This requires tracking captured cells within a single `makeMove` call (e.g., using a `std::unordered_set`).
* **(Optional Rule) Hits Enemy Cell**: The logic should support a configurable boolean `stopOnEnemy`. If `true`, the chain stops upon hitting a cell owned by the opponent. If `false` (the default), the enemy cell is captured and the chain continues.

### 5. Game Engine & Flow

#### 5.1. `GameEngine` Responsibilities

* The engine manages the main game loop.
* It holds shared pointers to the active `IGameMap`, `IGameLogic`, and `CLIView` instances.
* The main loop should:
    1.  Check if `gameLogic->isGameOver()` is true. If so, exit the loop.
    2.  Display the current board state via the view.
    3.  Request the next move from the `MoveProvider`.
    4.  Process the move using `gameLogic->makeMove()`.
    5.  Switch the current player.
    6.  Repeat.

#### 5.2. Input Abstraction (`MoveProvider`)

* To decouple the engine from user input for easier testing, a `MoveProvider` functional interface must be implemented.
* This will allow moves to be supplied by either the `CLIView` (for human players) or a scripted queue (for integration tests).

### 6. Win Conditions & Configuration

#### 6.1. `GameConfig` and `GameResult`

* The game must support configuration at startup via a `GameConfig` object, which specifies the `WinCondition` and `turnLimit`.
* When the game ends, a `GameResult` object should be produced containing the winner (or `nullptr` for a tie) and a string explaining the reason for the outcome.

#### 6.2. Implemented Win Conditions

The `isGameOver()` method in the game logic must check for the following conditions based on the active `GameConfig`:

* **`WinCondition::ELIMINATION`**: The game ends when one player has zero cells remaining on the board. This check should only be active after at least one turn has passed.
* **`WinCondition::TURN_LIMIT_MAJORITY`**: The game ends when the turn count reaches the `turnLimit` defined in `GameConfig`. The player controlling the most cells at that time is the winner. A tie is possible.

### 7. Testing Requirements

* **Unit Tests**: A comprehensive suite of unit tests using a framework like Google Test is required.
    * Test `GameMap` generation, ensuring correct cell count and neighbor linking for various sizes.
    * Test all `GameLogic` chain reaction stopping conditions individually.
    * Test multi-cell chain reactions and cycles.
* **Integration Tests**:
    * Full "play-through" tests with scripted moves must be created to validate the entire game flow from start to finish for each win condition.