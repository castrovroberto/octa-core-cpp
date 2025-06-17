### Phase P1: Core Logic - Granular Implementation Plan (Weeks 1-3)

This plan details the steps to implement the foundational data models and the initial version of the game logic, ensuring strict adherence to the `SRD v0.3.2` as the single source of truth. It involves refactoring existing files and creating new ones.

---

#### 1. Implement Core Data Models

This section focuses on updating and creating the fundamental data structures (`Coordinate`, `GameCell`, `Direction`, `CellState`, `Player`).

* **1.1. Review `Coordinate.h` and `CoordinateHash`**
    * **Action**: Confirm `include/octa-core/model/Coordinate.h` is aligned with `SRD v0.3.2` (it should already be).
        * Ensure `Coordinate` is a value type with `x` and `y` integers, and `operator==`/`operator!=` defined.
    * **Action**: Confirm the `CoordinateHash` struct (defined in `GraphGameMap.h`) is suitable for `std::unordered_map`. (It is).

* **1.2. Create `Enums.h` for `CellState` and `Player`**
    * **Action**: Create a new header file: `include/octa-core/core/Enums.h`.
    * **Content**: Define the `CellState` and `Player` enums as `uint8_t` to align with `SRD v0.3.2`:
        ```cpp
        #ifndef OCTA_CORE_ENUMS_H
        #define OCTA_CORE_ENUMS_H

        #include <cstdint> // For uint8_t

        /**
         * @enum CellState
         * @brief Defines the state/ownership of a game cell.
         */
        enum class CellState : uint8_t {
            NEUTRAL,
            PLAYER_1,
            PLAYER_2,
            BLOCKED
        };

        /**
         * @enum Player
         * @brief Represents the two players in the game.
         */
        enum class Player : uint8_t {
            PLAYER_1,
            PLAYER_2
        };

        #endif // OCTA_CORE_ENUMS_H
        ```

* **1.3. Create `Direction.h` with Helper Functions**
    * **Action**: Create a new header file: `include/octa-core/core/Direction.h`.
    * **Content**: Define the `Direction` enum and its helper functions as `uint8_t` and `inline` free functions as specified in `SRD v0.3.2` and `SRD v0.3.0`:
        ```cpp
        #ifndef OCTA_CORE_DIRECTION_H
        #define OCTA_CORE_DIRECTION_H

        #include <cstdint> // For uint8_t

        /**
         * @enum Direction
         * @brief Represents the 8 directions in an octagonal grid.
         */
        enum class Direction : uint8_t {
            N, NE, E, SE, S, SW, W, NW
        };

        /**
         * @brief Rotates a direction clockwise by 45 degrees.
         * @param d The current direction.
         * @return The new direction after clockwise rotation.
         */
        inline Direction rotateClockwise(Direction d) {
            return static_cast<Direction>((static_cast<int>(d) + 1) % 8);
        }

        /**
         * @brief Rotates a direction counter-clockwise by 45 degrees.
         * @param d The current direction.
         * @return The new direction after counter-clockwise rotation.
         */
        inline Direction rotateCounterClockwise(Direction d) {
            return static_cast<Direction>((static_cast<int>(d) + 8 - 1) % 8);
        }

        #endif // OCTA_CORE_DIRECTION_H
        ```

* **1.4. Refactor `GameCell.h`**
    * **Action**: Modify `include/octa-core/core/GameCell.h`.
    * **Changes**:
        * Update includes: Remove old `Direction` and `Player` definitions, include `Enums.h` and `Direction.h`.
        * **Crucial Update**: Change `Player occupant_;` to `CellState state_;`.
        * **Crucial Update**: Add `Direction direction_;` as a private member.
        * **Crucial Update**: Change `std::vector<std::shared_ptr<GameCell>> neighbors_;` to `std::array<std::weak_ptr<GameCell>, 8> neighbors_;` to prevent circular references as required by `SRD v0.3.2`.
        * Update method signatures/names:
            * Rename `getOccupant()` to `getState()` and `setOccupant()` to `setState(CellState state)`.
            * Add `getDirection()` and `setDirection(Direction dir)`.
            * Modify `getNeighbor(Direction direction)` to lock the `std::weak_ptr` before returning `std::shared_ptr`. If the `weak_ptr` is expired, return `nullptr`.
            * Modify `setNeighbor(Direction direction, std::shared_ptr<GameCell> neighbor)` to store the `std::weak_ptr` from the provided `shared_ptr`.
            * Remove `getAllNeighbors()` and `getValidNeighbors()` methods as they are not part of the `GameCell` definition in `SRD v0.3.2`.
        * Adjust constructor, documentation comments.

* **1.5. Update `GameCell.cpp`**
    * **Action**: Modify `src/core/GameCell.cpp`.
    * **Changes**:
        * Update includes to reflect `GameCell.h` changes.
        * Adjust the constructor to initialize `state_` (e.g., `CellState::NEUTRAL`) and `direction_` (e.g., `Direction::N`).
        * Implement the logic for `getState()`, `setState()`, `getDirection()`, `setDirection()`.
        * Implement `getNeighbor()` to safely convert `std::weak_ptr` to `std::shared_ptr` using `lock()`.
        * Implement `setNeighbor()` to assign `std::weak_ptr` from the input `std::shared_ptr`.
        * Remove implementations of `getAllNeighbors()` and `getValidNeighbors()`.

---

#### 2. Implement Map Abstraction and `GraphGameMap`

This section involves significant refactoring of the `IGameMap` interface and its `GraphGameMap` implementation to conform to `SRD v0.3.2`.

* **2.1. Refactor `IGameMap.h`**
    * **Action**: Modify `include/octa-core/map/IGameMap.h`.
    * **Changes**:
        * Include necessary headers for `Coordinate.h` and `GameCell.h`.
        * **Crucial Update**: Change method signatures to `at` and `size` as specified in `SRD v0.3.2`:
            ```cpp
            #ifndef OCTA_CORE_IGAMEMAP_H
            #define OCTA_CORE_IGAMEMAP_H

            #include <memory>
            #include <vector> // Still needed if getAllCells() is later re-introduced or for other purposes
            #include "../model/Coordinate.h"
            #include "../core/GameCell.h" // Ensure GameCell and its enums are visible

            class IGameMap {
            public:
                virtual ~IGameMap() = default;

                /**
                 * @brief Retrieves a cell at a specific coordinate.
                 * @param coord The coordinate of the cell to retrieve.
                 * @return A shared pointer to the GameCell, or nullptr if no cell exists.
                 */
                virtual std::shared_ptr<GameCell> at(const Coordinate& coord) const = 0;

                /**
                 * @brief Gets the total number of cells in the map.
                 * @return The total cell count.
                 */
                virtual size_t size() const = 0;
            };

            #endif // OCTA_CORE_IGAMEMAP_H
            ```
        * Remove `getAllCells()` and `getTotalCellCount()` from the interface.
        * Remove `getSize()` (the `size()` method from SRD v0.3.2 refers to total cell count, not radius).

* **2.2. Refactor `GraphGameMap.h` and `GraphGameMap.cpp`**
    * **Action**: Modify `include/octa-core/map/GraphGameMap.h`.
    * **Changes**:
        * Update method declarations to match `IGameMap` (`at`, `size`).
        * Ensure `CoordinateHash` remains valid and accessible.
    * **Action**: Modify `src/map/GraphGameMap.cpp`.
    * **Changes**:
        * Implement `at(const Coordinate& coord) const` to return `cells_.find(coord)->second` or `nullptr`.
        * Implement `size() const` to return `cells_.size()`.
        * Update `initializeMap()` and `linkCellNeighbors()` to work with the refactored `GameCell` (especially handling `std::weak_ptr` for neighbors and initializing `CellState`/`Direction`).
        * Remove implementations of `getAllCells()` and `getTotalCellCount()`.
        * Remove implementation of `getSize()`.

* **2.3. Refactor `ArrayGameMap.h` and `ArrayGameMap.cpp` (Placeholder Update)**
    * **Action**: Modify `include/octa-core/map/ArrayGameMap.h` and `src/map/ArrayGameMap.cpp`.
    * **Changes**: Update the placeholder method signatures to match the new `IGameMap` interface (`at`, `size`). Keep the `throw std::runtime_error("...")` implementations.

* **2.4. Update `GameEngine` and `Main.cpp` for `IGameMap` Changes**
    * **Action**: Modify `include/octa-core/engine/GameEngine.h`, `src/engine/GameEngine.cpp`, and `src/Main.cpp`.
    * **Changes**:
        * Adjust calls to `IGameMap` methods: `gameMap_->getCell(...)` becomes `gameMap_->at(...)`, `gameMap_->getSize()` is removed, `gameMap_->getTotalCellCount()` becomes `gameMap_->size()`.
        * Update `GameEngine::displayMapInfo()` to reflect these changes.

---

#### 3. Implement Game Logic (Tier 0 - `VALIDATE_ONLY`)

This involves creating the core game logic components as specified in `SRD v0.3.2`.

* **3.1. Create `GameConfig.h` for `GameConfig` and `GameResult`**
    * **Action**: Create a new header file: `include/octa-core/model/GameConfig.h`.
    * **Content**: Define the `WinCondition`, `SafetyLevel`, `GameConfig`, and `GameResult` structs/enums:
        ```cpp
        #ifndef OCTA_CORE_GAMECONFIG_H
        #define OCTA_CORE_GAMECONFIG_H

        #include <cstdint> // For uint8_t
        #include <optional>
        #include <string>
        #include "../core/Enums.h" // For Player enum

        /**
         * @enum WinCondition
         * @brief Defines the conditions for game termination.
         */
        enum class WinCondition { ELIMINATION, TURN_LIMIT_MAJORITY };

        /**
         * @enum SafetyLevel
         * @brief Defines the level of exception safety for game operations.
         */
        enum class SafetyLevel { VALIDATE_ONLY, LIGHT_UNDO, FULL_ROLLBACK };

        /**
         * @struct GameConfig
         * @brief Configuration settings for a game session.
         */
        struct GameConfig {
            WinCondition winCondition = WinCondition::ELIMINATION;
            int          turnLimit    = 60;
            bool         stopOnEnemy  = false;
            SafetyLevel  safetyLevel  = SafetyLevel::VALIDATE_ONLY;
        };

        /**
         * @struct GameResult
         * @brief Encapsulates the outcome of a game.
         */
        struct GameResult {
            std::optional<Player> winner;   // nullopt => draw
            std::string           reason;   // human-readable
        };

        #endif // OCTA_CORE_GAMECONFIG_H
        ```

* **3.2. Create `IGameLogic.h` Interface**
    * **Action**: Create a new header file: `include/octa-core/engine/IGameLogic.h`.
    * **Content**: Define the `IGameLogic` interface as specified in `SRD v0.3.2`:
        ```cpp
        #ifndef OCTA_CORE_IGAMELOGIC_H
        #define OCTA_CORE_IGAMELOGIC_H

        #include <memory>
        #include "../core/GameCell.h" // For GameCell
        #include "../core/Enums.h"    // For Player
        #include "../model/GameConfig.h" // For GameConfig, GameResult

        class IGameLogic {
        public:
            virtual ~IGameLogic() = default;
            virtual bool        isValidMove(std::shared_ptr<const GameCell> cell, Player player) const = 0;
            virtual GameResult  makeMove   (std::shared_ptr<GameCell> cell,       Player player)      = 0;
            virtual bool        isGameOver () const = 0;
            virtual Player      getCurrentPlayer() const = 0;
            virtual void        switchPlayer()            = 0;
            virtual int         getTurnCount()      const = 0;
            virtual const GameConfig& getConfig()   const = 0;
        };

        #endif // OCTA_CORE_IGAMELOGIC_H
        ```

* **3.3. Create `OctaGameLogic.h` and `OctaGameLogic.cpp`**
    * **Action**: Create `include/octa-core/engine/OctaGameLogic.h` and `src/engine/OctaGameLogic.cpp`.
    * **`OctaGameLogic.h` Content**:
        ```cpp
        #ifndef OCTA_CORE_OCTAGAMELOGIC_H
        #define OCTA_CORE_OCTAGAMELOGIC_H

        #include "IGameLogic.h"
        #include "../map/IGameMap.h" // GameLogic needs access to the map
        #include <unordered_set> // For capturedThisTurn
        // Include any custom exceptions here (will be defined in P3 or later phases)

        class OctaGameLogic : public IGameLogic {
        public:
            explicit OctaGameLogic(std::shared_ptr<IGameMap> map, const GameConfig& config);
            ~OctaGameLogic() override = default;

            bool        isValidMove(std::shared_ptr<const GameCell> cell, Player player) const override;
            GameResult  makeMove   (std::shared_ptr<GameCell> cell,       Player player)      override;
            bool        isGameOver () const override;
            Player      getCurrentPlayer() const override;
            void        switchPlayer()            override;
            int         getTurnCount()      const override;
            const GameConfig& getConfig()   const override;

        private:
            std::shared_ptr<IGameMap> gameMap_;
            Player                       currentPlayer_ = Player::PLAYER_1;
            int                          turnCount_     = 0;
            GameConfig                   config_;
            std::optional<GameResult>    gameResult_;

            // Helper for makeMove (chain reaction propagation)
            void propagateChain(std::shared_ptr<GameCell> startCell, Player player, std::unordered_set<GameCell*>& capturedThisTurn);
        };

        #endif // OCTA_CORE_OCTAGAMELOGIC_H
        ```
    * **`OctaGameLogic.cpp` Content**:
        * **Implement Constructor**: Initialize `gameMap_` and `config_`.
        * **Implement `isValidMove`**: Return `true` if `cell->getState() == player.toCellState()` (you might need a helper to convert `Player` to `CellState`).
        * **Implement `makeMove`**:
            * Call `isValidMove` and, if `false`, *for P1, throw a basic `std::invalid_argument`* (custom exceptions will be for P3).
            * Set up `std::unordered_set<GameCell*> capturedThisTurn;`.
            * Rotate initial cell: `cell->setDirection(rotateClockwise(cell->getDirection()));`
            * Start propagation: `propagateChain(cell, player, capturedThisTurn);`
            * Increment `turnCount_`.
            * Determine `GameResult` (basic version for now, full win conditions later).
        * **Implement `propagateChain`**: This is the loop.
            * In each iteration:
                * Get the next neighbor based on the cell's `direction_` (locking `weak_ptr`).
                * Apply stopping conditions from `SRD v0.3.2` (off-grid, owned by current player, already captured, `stopOnEnemy`). Break the loop if met.
                * If not stopping: Capture the cell (`cell->setState(player.toCellState())`). Rotate its arrow. Add to `capturedThisTurn`. Set next cell.
        * **Implement `isGameOver`**: Basic check for `turnCount_ >= config_.turnLimit` or `gameResult_` is set.
        * **Implement `getCurrentPlayer`**: Return `currentPlayer_`.
        * **Implement `switchPlayer`**: Toggle `currentPlayer_`.
        * **Implement `getTurnCount`**: Return `turnCount_`.
        * **Implement `getConfig`**: Return `config_`.

* **3.4. Update `GameEngine` for `IGameLogic` Integration**
    * **Action**: Modify `include/octa-core/engine/GameEngine.h` and `src/engine/GameEngine.cpp`.
    * **Changes**:
        * Add `std::shared_ptr<IGameLogic> gameLogic_;` as a member.
        * Update the constructor to accept `std::shared_ptr<IGameLogic> logic`.
        * Modify `run()` to integrate `gameLogic_`, including a basic game loop.

---

#### 4. Unit Testing (≥70% coverage)

This section outlines the tests to be written using Google Test to verify the implemented components.

* **4.1. Update `test_main.cpp`**
    * **Action**: Modify `tests/test_main.cpp`.
    * **Changes**:
        * Add includes for all new and modified headers: `Direction.h`, `Enums.h`, `GameCell.h`, `IGameMap.h`, `GraphGameMap.h`, `IGameLogic.h`, `OctaGameLogic.h`, `GameConfig.h`.
        * **Test `Direction` Helper Functions**:
            ```cpp
            TEST(DirectionTests, RotateClockwise) {
                EXPECT_EQ(rotateClockwise(Direction::N), Direction::NE);
                EXPECT_EQ(rotateClockwise(Direction::NW), Direction::N);
                // Add all 8 directions
            }

            TEST(DirectionTests, RotateCounterClockwise) {
                EXPECT_EQ(rotateCounterClockwise(Direction::N), Direction::NW);
                EXPECT_EQ(rotateCounterClockwise(Direction::NE), Direction::N);
                // Add all 8 directions
            }
            ```
        * **Test `GameCell` Basic Functionality**:
            ```cpp
            TEST(GameCellTests, StateAndDirection) {
                Coordinate c(0, 0);
                GameCell cell(c);
                EXPECT_EQ(cell.getState(), CellState::NEUTRAL);
                EXPECT_EQ(cell.getDirection(), Direction::N); // Default
                cell.setState(CellState::PLAYER_1);
                EXPECT_EQ(cell.getState(), CellState::PLAYER_1);
                cell.setDirection(Direction::SE);
                EXPECT_EQ(cell.getDirection(), Direction::SE);
            }
            // Add tests for weak_ptr neighbor linking
            ```
        * **Test `GraphGameMap` Initialization**:
            ```cpp
            TEST(GraphGameMapTests, Initialization) {
                GraphGameMap map(2); // 5x5 grid
                EXPECT_EQ(map.size(), 25);
                EXPECT_NE(map.at(Coordinate(0, 0)), nullptr);
                EXPECT_EQ(map.at(Coordinate(3, 0)), nullptr); // Out of bounds

                // Test neighbor linking for a central cell
                auto centerCell = map.at(Coordinate(0, 0));
                ASSERT_NE(centerCell, nullptr);
                EXPECT_NE(centerCell->getNeighbor(Direction::N), nullptr);
                EXPECT_EQ(centerCell->getNeighbor(Direction::N)->getCoordinate(), Coordinate(0, 1));
                // Add tests for all 8 neighbors and boundary cells
            }
            ```

* **4.2. Create `GameLogicTests.cpp`**
    * **Action**: Create a new file `tests/GameLogicTests.cpp` (or `tests/engine/GameLogicTests.cpp`).
    * **Content**:
        * **Test `isValidMove`**:
            ```cpp
            TEST(OctaGameLogicTests, IsValidMove) {
                auto map = std::make_shared<GraphGameMap>(1);
                GameConfig config;
                OctaGameLogic logic(map, config);

                auto cell = map->at(Coordinate(0,0));
                cell->setState(CellState::PLAYER_1);
                EXPECT_TRUE(logic.isValidMove(cell, Player::PLAYER_1));
                EXPECT_FALSE(logic.isValidMove(cell, Player::PLAYER_2));
                cell->setState(CellState::NEUTRAL);
                EXPECT_FALSE(logic.isValidMove(cell, Player::PLAYER_1));
            }
            ```
        * **Test `makeMove` (Tier 0 - Basic Chain Reaction and Stopping Conditions)**:
            ```cpp
            TEST(OctaGameLogicTests, MakeMove_BasicRotation) {
                auto map = std::make_shared<GraphGameMap>(1);
                GameConfig config;
                OctaGameLogic logic(map, config);

                auto cell = map->at(Coordinate(0,0));
                cell->setState(CellState::PLAYER_1);
                cell->setDirection(Direction::N);

                logic.makeMove(cell, Player::PLAYER_1);
                EXPECT_EQ(cell->getDirection(), Direction::NE);
            }

            TEST(OctaGameLogicTests, MakeMove_SimpleChain) {
                auto map = std::make_shared<GraphGameMap>(1);
                GameConfig config;
                OctaGameLogic logic(map, config);

                auto cell1 = map->at(Coordinate(0,0)); // Player 1, N
                auto cell2 = map->at(Coordinate(0,1)); // Neutral
                cell1->setState(CellState::PLAYER_1);
                cell1->setDirection(Direction::N);

                logic.makeMove(cell1, Player::PLAYER_1);
                EXPECT_EQ(cell1->getDirection(), Direction::NE);
                EXPECT_EQ(cell2->getState(), CellState::PLAYER_1);
                EXPECT_EQ(logic.getTurnCount(), 1);
            }

            TEST(OctaGameLogicTests, MakeMove_StopOnGridEdge) {
                auto map = std::make_shared<GraphGameMap>(0); // 1x1 map
                GameConfig config;
                OctaGameLogic logic(map, config);

                auto cell = map->at(Coordinate(0,0));
                cell->setState(CellState::PLAYER_1);
                cell->setDirection(Direction::N); // Points off grid

                logic.makeMove(cell, Player::PLAYER_1);
                EXPECT_EQ(cell->getDirection(), Direction::NE); // Initial cell rotates
                // No chain reaction occurs
                EXPECT_EQ(logic.getTurnCount(), 1);
            }

            TEST(OctaGameLogicTests, MakeMove_StopOnOwnCell) {
                auto map = std::make_shared<GraphGameMap>(1);
                GameConfig config;
                OctaGameLogic logic(map, config);

                auto cell1 = map->at(Coordinate(0,0));
                auto cell2 = map->at(Coordinate(0,1));
                cell1->setState(CellState::PLAYER_1);
                cell1->setDirection(Direction::N);
                cell2->setState(CellState::PLAYER_1); // Own cell

                logic.makeMove(cell1, Player::PLAYER_1);
                EXPECT_EQ(cell1->getDirection(), Direction::NE);
                EXPECT_EQ(cell2->getState(), CellState::PLAYER_1); // Still owned by P1, not re-captured
                EXPECT_EQ(logic.getTurnCount(), 1);
            }
            // Add tests for StopOnLoop and StopOnEnemy (if config.stopOnEnemy is true)
            ```

This detailed plan provides actionable steps for Phase P1, aligning the codebase with the `SRD v0.3.2` and setting up a solid foundation for subsequent development phases.

---
### Sources
1.  **Octa-Core C++ Port: Game Mechanics & Requirements (v3.0)**
2.  **Octa-Core C++ Port – Final Requirements Specification (v3.2)**