### Phase P2: Light Undo & Benchmarking - Granular Implementation Plan (Weeks 4-5)

This phase focuses on enhancing the game's core logic with a basic rollback mechanism (`LIGHT_UNDO`) and establishing performance measurement and optimization practices.

---

#### 1. Implement Tier 1 Safety (`LIGHT_UNDO`)

This sub-phase implements a transactional capability within `makeMove` to revert changes if an operation fails, providing the "basic exception safety guarantee" outlined in `SRD v0.3.2`.

* **1.1. Define `CellChange` Struct for Undo Information**
    * **Action**: Create a new header file at `include/octa-core/core/CellChange.h`.
    * **Content**: Define a struct to record the necessary state for undo operations. This struct will store a shared pointer to the `GameCell` that was modified, along with its state and direction *before* the modification.
        ```cpp
        #ifndef OCTA_CORE_CELLCHANGE_H
        #define OCTA_CORE_CELLCHANGE_H

        #include "../model/Coordinate.h"
        #include "Enums.h"
        #include "Direction.h"
        #include <memory>

        // Forward declaration to avoid circular dependency with GameCell.h
        class GameCell;

        /**
         * @struct CellChange
         * @brief Represents a record of a GameCell's state and direction before a modification,
         * used for rolling back changes.
         */
        struct CellChange {
            std::shared_ptr<GameCell> cell;       ///< Pointer to the modified cell.
            CellState                oldState;     ///< The cell's state before modification.
            Direction                oldDirection; ///< The cell's direction before modification.

            /**
             * @brief Constructs a CellChange record.
             * @param c The GameCell that was modified.
             * @param os The cell's original state.
             * @param od The cell's original direction.
             */
            CellChange(std::shared_ptr<GameCell> c, CellState os, Direction od)
                : cell(c), oldState(os), oldDirection(od) {}
        };

        #endif // OCTA_CORE_CELLCHANGE_H
        ```

* **1.2. Modify `OctaGameLogic::makeMove` for `LIGHT_UNDO` Logic**
    * **Action**: Modify the implementation file `src/engine/OctaGameLogic.cpp`.
    * **Changes**:
        1.  **Include `CellChange.h`**: Add `#include "../core/CellChange.h"` to `OctaGameLogic.cpp`.
        2.  **Declare `undoLog`**: Inside the `makeMove` method, declare a `std::vector<CellChange> undoLog;`. This vector will accumulate changes for potential rollback.
        3.  **Conditional Recording**: Before *any* modification is made to a `GameCell`'s `state_` or `direction_` (both the initially selected cell and any cells captured during propagation), check if `config_.safetyLevel == SafetyLevel::LIGHT_UNDO`. If true, record the cell's current state and direction into `undoLog`:
            ```cpp
            // Example: Before modifying a cell's state/direction
            if (config_.safetyLevel == SafetyLevel::LIGHT_UNDO) {
                undoLog.emplace_back(cellToModify, cellToModify->getState(), cellToModify->getDirection());
            }
            // Now, safely perform the modification:
            // cellToModify->setState(...);
            // cellToModify->setDirection(...);
            ```
        4.  **Wrap in `try...catch`**: Enclose the entire core logic of `makeMove` (from initial move validation and rotation, through the chain propagation loop, up to incrementing the turn count) within a `try` block.
        5.  **Implement Rollback in `catch` block**: In the corresponding `catch` block (e.g., `catch (const std::exception& e)`), iterate through the `undoLog` in reverse order. For each `CellChange` record, revert the `cell`'s `state_` and `direction_` back to `oldState` and `oldDirection` respectively. After rolling back, re-throw the caught exception to propagate the error.
            ```cpp
            try {
                // ... entire makeMove logic (validation, rotation, propagation, turn increment)
            } catch (const std::exception& e) {
                // Rollback changes
                for (auto it = undoLog.rbegin(); it != undoLog.rend(); ++it) {
                    if (it->cell) { // Ensure the shared_ptr is still valid
                        it->cell->setState(it->oldState);
                        it->cell->setDirection(it->oldDirection);
                    }
                }
                throw; // Re-throw the original exception
            }
            ```

---

#### 2. Integrate Google Benchmark

This sub-phase focuses on creating performance test cases to measure the `makeMove` function's latency.

* **2.1. Add `makeMove` Benchmarks**
    * **Action**: Create a new file `bench/OctaCoreBenchmarks.cpp`.
    * **Action**: Update `CMakeLists.txt` to add `bench/OctaCoreBenchmarks.cpp` to the `perf_suite` executable target.
    * **Content**:
        * Include necessary headers: `benchmark/benchmark.h`, `octa-core/engine/OctaGameLogic.h`, `octa-core/map/GraphGameMap.h`, `octa-core/model/GameConfig.h`, etc.
        * Implement helper functions to set up specific game scenarios for benchmarks (e.g., `SetupSmallMapScenario`, `SetupLongChainScenario`). These helpers should create `std::shared_ptr<IGameMap>` and `OctaGameLogic` instances with predefined cell states to reliably produce short, medium, or long chain reactions.
        * Define benchmark functions using the `BENCHMARK` macro for various `makeMove` scenarios:
            * `BM_MakeMove_ShortChain`: Measures `makeMove` on a small map (e.g., size 2-3) where the chain reaction involves only a few cells (e.g., < 10 cells).
            * `BM_MakeMove_MediumChain`: Measures `makeMove` on a moderately sized map (e.g., size 5-10) with a chain reaction involving a moderate number of cells (e.g., 20-50 cells).
            * `BM_MakeMove_LongChain`: Measures `makeMove` on a larger map (e.g., size 20-50, or more for stress testing) specifically designed to generate very long chain reactions (e.g., hundreds of cells), as per `SRD v0.3.2`'s `Max chain length ≤ 1,000 cells` target.
            * `BM_MakeMove_NoChain`: Measures `makeMove` where the chain reaction immediately stops (e.g., selected cell points off-grid, or to an already-owned cell).
        * **Example for `BM_MakeMove_LongChain`**:
            ```cpp
            #include <benchmark/benchmark.h>
            #include "octa-core/engine/OctaGameLogic.h"
            #include "octa-core/map/GraphGameMap.h"
            #include "octa-core/model/GameConfig.h"
            #include "octa-core/core/Direction.h" // Needed for Direction enum

            // Helper function to set up a linear chain for benchmarking
            static std::shared_ptr<OctaGameLogic> CreateLinearChainLogic(int mapRadius, int chainLength) {
                // Adjust mapRadius to ensure the chain fits (e.g., mapRadius = chainLength / 2 + 1)
                auto map = std::make_shared<GraphGameMap>(mapRadius);
                GameConfig config;
                config.stopOnEnemy = false; // Allow chains to propagate through enemy cells if needed
                config.safetyLevel = SafetyLevel::LIGHT_UNDO; // Test with LIGHT_UNDO enabled

                for (int i = 0; i < chainLength; ++i) {
                    auto currentCell = map->at(Coordinate(i, 0));
                    if (currentCell) {
                        currentCell->setState(CellState::NEUTRAL);
                        currentCell->setDirection(Direction::E); // Point east to the next cell
                    }
                }
                // Set the starting cell for Player 1
                auto startCell = map->at(Coordinate(0, 0));
                if (startCell) {
                    startCell->setState(CellState::PLAYER_1);
                    startCell->setDirection(Direction::E); // Initial move direction
                }
                return std::make_shared<OctaGameLogic>(map, config);
            }

            // Benchmark function for long chains
            static void BM_MakeMove_LongChain(benchmark::State& state) {
                // state.range(0) will be the 'chainLength' passed by the Range argument
                const int mapRadius = state.range(0) / 2 + 2; // Map size large enough to contain chain
                auto logic = CreateLinearChainLogic(mapRadius, state.range(0));
                auto startCell = logic->getConfig().winCondition == WinCondition::ELIMINATION ? nullptr : logic->getMap()->at(Coordinate(0, 0)); // Get start cell

                if (!startCell) {
                    state.SkipWithError("Could not initialize start cell for benchmark.");
                    return;
                }

                for (auto _ : state) {
                    // Re-create the logic for each iteration to ensure a clean state
                    state.PauseTiming();
                    logic = CreateLinearChainLogic(mapRadius, state.range(0));
                    startCell = logic->getMap()->at(Coordinate(0, 0));
                    if (!startCell) { // Check again after recreation
                         state.SkipWithError("Start cell became null after re-creation.");
                         continue;
                    }
                    state.ResumeTiming();

                    logic->makeMove(startCell, Player::PLAYER_1);
                }
            }
            // Register the benchmark with different chain lengths
            BENCHMARK(BM_MakeMove_LongChain)->RangeMultiplier(2)->Range(10, 1024); // Test from 10 to 1024 cells in chain
            ```

---

#### 3. Achieve Performance Targets & CI Integration

This sub-phase focuses on optimizing the code to meet performance metrics and ensuring these metrics are tracked by the CI pipeline.

* **3.1. Run Benchmarks and Profile Code**
    * **Action (Manual)**: After implementing benchmarks, compile the `perf_suite` executable (e.g., using `cmake --build build --preset dev-release-build`).
    * **Action (Manual)**: Run the benchmarks from the command line (e.g., `./build/Release/perf_suite` on Linux/macOS, or `.\build\Release\perf_suite.exe` on Windows).
    * **Action (Manual)**: Use profiling tools (e.g., `gprof` on Linux, Valgrind's `Callgrind`, Visual Studio Profiler on Windows) to identify performance bottlenecks within the `makeMove` and chain propagation logic.

* **3.2. Optimize `makeMove` and Related Data Structures**
    * **Action (Development)**: Based on profiling results, identify and implement optimizations. This might involve:
        * Revisiting the chain propagation loop for efficiency.
        * Optimizing cell access patterns within the `GraphGameMap`.
        * Minimizing memory allocations within the hot path of `makeMove`.
        * Ensuring `std::unordered_set` for `capturedThisTurn` is performing optimally.
    * **Goal**: Meet the latency targets specified in `SRD v0.3.2`: `Median makeMove() latency ≤ 2 ms` and `99th percentile latency ≤ 5 ms`.

* **3.3. Update CI Pipeline to Run Benchmarks and Fail on Regression**
    * **Action**: Modify the GitHub Actions workflow file: `.github/workflows/ci.yml`.
    * **Changes**:
        1.  **Enable Benchmark Dependencies**: In the `Install dependencies` step for the `build-and-test` job, ensure `enable_benchmarks` is set to `True`:
            ```yaml
            - name: Install dependencies
              run: |
                conan install . --build=missing -s build_type=${{ env.BUILD_TYPE }} \
                  -o enable_testing=True -o enable_benchmarks=True # <- Change this to True
            ```
        2.  **Run Benchmark Suite**: Add a step *after* the `Build` step to execute the performance benchmarks. This will output results in JSON format, which can be parsed later for regression analysis.
            ```yaml
            - name: Run performance benchmarks
              if: matrix.config.os == 'ubuntu-latest' # Run benchmarks on a consistent platform for reliable metrics
              run: |
                # The path to the executable will vary slightly by OS and build type.
                # Adjust as necessary based on your CMake setup and environment.
                # Example for Linux: ./build/Release/perf_suite
                # Example for Windows: .\build\Release\perf_suite.exe
                # Use --benchmark_format=json and --benchmark_out to save results for analysis.
                ./build/${{ env.BUILD_TYPE }}/perf_suite --benchmark_format=json --benchmark_out=${{ github.workspace }}/benchmark_results.json
            ```
        3.  **Implement Regression Gating (Placeholder/Future)**: The `SRD v0.3.2` states "CI fails if any metric regresses". For Phase P2, simply running the benchmarks is the main goal. Full regression gating (parsing JSON, comparing to baselines, failing on deviation) is a more advanced CI task that might be fully implemented in a later phase (e.g., P3 or P4) or require a dedicated GitHub Action/script. For now, the output of `perf_suite` will be available in the CI logs.

---