### Phase P0: Scaffold (Week 1)

This phase focuses on setting up the foundational structure of the project to ensure all subsequent development is smooth, consistent, and automated.

* **1. Initialize Git Repository & Project Structure**:
    * Create the main project repository.
    * Establish the directory structure:
        * `src/`: For all `.cpp` implementation files.
        * `include/octa-core/`: For all public-facing `.h` header files.
        * `tests/`: For all test-related source files.
        * `docs/`: For all markdown and Doxygen configuration files.
        * `cmake/`: For CMake helper scripts.
        * `build/`: Add to `.gitignore`.

* **2. Setup `CMakeLists.txt`**:
    * Create the root `CMakeLists.txt`.
    * Set the project name to `OctaCore` and require C++20.
    * Define the main executable target (`octa-core`).
    * Define the test runner executable target (`run_tests`).
    * Configure include directories using `target_include_directories` to point to `include/`.
    * Add CMake presets for development and packaging (`dev-debug.json`, `package-unity.json`, etc.) as specified in SRD v3.2.

* **3. Integrate Dependency Management (Conan)**:
    * Create a `conanfile.py` to manage dependencies.
    * Add **Google Test** and **spdlog** as initial dependencies.
    * Modify `CMakeLists.txt` to integrate with Conan, ensuring it can find the downloaded dependencies.

* **4. Establish Code Style and Formatting**:
    * Create a `.clang-format` file at the repository root, configured to match the **Google C++ Style Guide**.
    * Configure your IDE (e.g., VSCode, CLion) to automatically format code on save using this file.

* **5. Implement CI Skeleton (GitHub Actions)**:
    * Create a `.github/workflows/ci.yml` file.
    * Set up a matrix build strategy for `ubuntu-latest`, `windows-latest`, and `macos-latest`.
    * Add initial steps to checkout code, configure CMake, and build the project. This will initially fail but confirms the workflow is triggered correctly.

### Phase P1: Core Logic (Weeks 1-3)

This phase implements the fundamental data models and the simplest version of the game logic.

* **1. Implement Core Data Models**:
    * `Coordinate.h`: Create the `Coordinate` struct.
    * `GameCell.h`: Define `Direction`, `CellState`, and `Player` enums. Implement the `GameCell` class with `coordinate_`, `state_`, `direction_`, and `neighbors_` members. Neighbor pointers must be `std::weak_ptr` to prevent cycles.
    * Implement helper functions `rotateClockwise` and `rotateCounterClockwise` in `Direction.h`.

* **2. Implement Map Abstraction and `GraphGameMap`**:
    * `IGameMap.h`: Create the `IGameMap` interface with its pure virtual functions (`at`, `size`).
    * `GraphGameMap.h/cpp`: Implement the `GraphGameMap` class, inheriting from `IGameMap`.
        * Use `std::unordered_map` for cell storage with a custom `CoordinateHash`.
        * Implement the map initialization logic to create cells and link neighbors bidirectionally.

* **3. Implement Game Logic (Tier 0 - `VALIDATE_ONLY`)**:
    * `IGameLogic.h`: Define the `IGameLogic` interface.
    * `OctaGameLogic.h/cpp`: Implement `OctaGameLogic`, owning `currentPlayer_`, `turnCount_`, and `config_`.
    * Implement `isValidMove` as specified.
    * Implement the `makeMove` algorithm using the `SafetyLevel::VALIDATE_ONLY` strategy (simulate on a shadow map or copy).

* **4. Unit Testing (≥70% coverage)**:
    * Write tests for `GraphGameMap` initialization (correct cell count, valid neighbor linking).
    * Test the `rotateClockwise` function exhaustively.
    * Write focused unit tests for every stopping condition in the `makeMove` chain reaction logic.

### Phase P2: Light Undo & Benchmarking (Weeks 4-5)

This phase enhances the core logic with a more robust safety mechanism and introduces performance measurement.

* **1. Implement Tier 1 Safety (`LIGHT_UNDO`)**:
    * In `OctaGameLogic::makeMove`, add logic to detect `GameConfig::safetyLevel`.
    * If `LIGHT_UNDO` is active, create a "diff list" of `{coord, oldState, oldDir}` before modifying cells.
    * Wrap the move execution in a `try...catch` block. On failure, iterate through the diff list to revert the changes.

* **2. Integrate Google Benchmark**:
    * Add a `bench/` directory.
    * Create a new CMake target for the performance suite.
    * Write initial benchmarks for the `makeMove` function under various scenarios (e.g., short chain, long chain).

* **3. Achieve Performance Targets**:
    * Run benchmarks and profile the code.
    * Optimize `makeMove` and related data structures to meet the specified latency targets (Median ≤ 2 ms, 99th percentile ≤ 5 ms).
    * Update the CI pipeline to run benchmarks and fail on regression.

### Phase P3: Full Rollback & Thread Safety (Weeks 5-6)

This phase completes the core logic with the most robust safety features and prepares the library for concurrent use.

* **1. Implement Tier 2 Safety (`FULL_ROLLBACK`)**:
    * Design and implement the **Command Pattern** for move execution. Create an `UndoableCommand` interface and concrete command classes for cell state and direction changes.
    * Refactor `makeMove` to use the `FULL_ROLLBACK` strategy, creating a composite command object for the entire move transaction.

* **2. Implement Thread-Safe Adapter**:
    * Create a `ThreadSafeGameMap` class that wraps an `IGameMap`.
    * Use a `std::scoped_lock` on a `std::mutex` to protect write operations (`makeMove`). Read-only queries should remain lock-free.
    * Add the `OCTA_THREAD_SAFE` compile-time flag in CMake to enable/disable this feature.

* **3. Memory Leak Detection**:
    * Update the CI pipeline to run the test suite under **Valgrind** (on Linux) and **Dr. Memory** (on Windows).
    * Fix any identified memory leaks until the tools report zero "definite" leaks.

### Phase P4: Engine Bindings & Serialization (Weeks 6-7)

This phase focuses on making the library usable by external game engines and for saving/loading state.

* **1. Create a C API Wrapper**:
    * Design a C-style API that exposes the core library functionality. This is crucial for both Godot and Unity.
    * This API will manage object lifetimes (e.g., `octa_create_game`, `octa_destroy_game`) and translate C++ exceptions into error codes.

* **2. Implement Godot GDNative Bindings**:
    * Create a new `godot-addon/` directory.
    * Write the `octa.gdextension` file.
    * Implement the C++ side of the GDNative interface, calling the C API wrapper.

* **3. Implement Unity P-Invoke Bindings**:
    * Create a `unity-package/` directory.
    * Write a C# wrapper class that uses `[DllImport]` to call the functions in the compiled shared library (`.dll`/`.so`) via the C API.

* **4. Implement Serialization**:
    * Integrate a JSON library (e.g., nlohmann/json).
    * Implement the `serialize(StateFormat::JSON)` and `deserialize` functions for saving and loading game state.

### Phase P5: Polish & Documentation (Week 8)

The final phase is dedicated to documentation, tutorials, and ensuring a high-quality developer experience.

* **1. Doxygen Documentation**:
    * Ensure every public class, method, and enum has Doxygen-style comments with examples.
    * Configure and run Doxygen to generate HTML documentation.
    * Add a CI step to fail the build if Doxygen produces warnings.

* **2. Write Tutorials**:
    * Create the "Engine-less Hello World," "Integrate with Godot," and "Integrate with Unity" tutorials as markdown files in the `docs/` directory.

* **3. Finalize CI/CD Pipeline**:
    * Add steps to automatically package the Godot addon and Unity package as artifacts.
    * Set up a GitHub Pages deployment for the Doxygen documentation.
    * Ensure all tests, benchmarks, and coverage checks are green.

* **4. Acceptance Criteria Check**:
    * Perform a final review against the acceptance criteria: all tests passing, coverage ≥ 90%, engine sample scenes running, and Doxygen documentation published without warnings.