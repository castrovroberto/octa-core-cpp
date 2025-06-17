Below is the **complete, self-contained “Final Requirements Specification v3.2”** for the Octa-Core C++ port.
You can copy-paste it directly into version control as `SRD_v3.2.md`.

---

# Octa-Core C++ Port – Final Requirements Specification (v3.2)

**Status:** Approved & ready for development
**Date:** 17 June 2025
**Supersedes:** v3.1 (gap-analysis edition)
**Applies to:** Octa-Core codebase, engine bindings, build & CI infrastructure

---

## Table of Contents

1. [Introduction](#1-introduction)
2. [Core Concepts & Glossary](#2-core-concepts--glossary)
3. [System Architecture & Data Models](#3-system-architecture--data-models)
4. [Game Logic](#4-game-logic)
5. [Game Engine & Flow](#5-game-engine--flow)
6. [Configuration & Win Conditions](#6-configuration--win-conditions)
7. [Error Handling & Exception Safety](#7-error-handling--exception-safety)
8. [Memory Management Policy](#8-memory-management-policy)
9. [Performance & Resource Targets](#9-performance--resource-targets)
10. [Thread Safety & Concurrency](#10-thread-safety--concurrency)
11. [Serialization & Persistence](#11-serialization--persistence)
12. [Debugging, Logging & Diagnostics](#12-debugging-logging--diagnostics)
13. [Development Process, Testing & CI](#13-development-process-testing--ci)
14. [Build, Packaging & Distribution](#14-build-packaging--distribution)
15. [Documentation Standards](#15-documentation-standards)
16. [Implementation Roadmap](#16-implementation-roadmap)
17. [Risk & Mitigation](#17-risk--mitigation)
18. [Acceptance Criteria](#18-acceptance-criteria)
19. [Next Steps](#19-next-steps)

---

## 1  Introduction

Port the existing Java **octa-core** game-logic library to **modern C++ (C++20)** as a **modular, scalable, engine-agnostic SDK** with first-class bindings for **Godot 4** and **Unity 2022 LTS**.

The library must deliver:

* **Deterministic, robust gameplay rules**
* **Low-latency move processing** suitable for real-time engines
* **Lightweight memory footprint** for mobile/embedded targets
* **Complete unit/integration tests + CI gates**
* **Straightforward engine integration (C API / P-Invoke)**

---

## 2  Core Concepts & Glossary

| Term                    | Definition                                                          |
| ----------------------- | ------------------------------------------------------------------- |
| **Cell**                | Fundamental board unit, holding a `State` and an `Arrow Direction`. |
| **State (`CellState`)** | `NEUTRAL`, `PLAYER_1`, `PLAYER_2`, `BLOCKED`.                       |
| **Arrow Direction**     | One of 8 compass directions in an octagonal grid.                   |
| **Chain Reaction**      | Cascading capture of adjacent cells triggered by a move.            |
| **Turn**                | One player action; increments `turnCount`.                          |
| **Game Map**            | Graph of interconnected `GameCell`s.                                |

---

## 3  System Architecture & Data Models

### 3.1  Map Abstraction (`IGameMap`)

```cpp
class IGameMap {
public:
    virtual ~IGameMap() = default;
    virtual std::shared_ptr<GameCell>  at(const Coordinate&) const = 0;
    virtual size_t                     size()                const = 0;
};
```

* **Concrete:** `GraphGameMap` using
  `std::unordered_map<Coordinate, std::shared_ptr<GameCell>, CoordinateHash>`.
* **Initialization:** square from `-size … +size` on both axes → `(2·size+1)²` cells.
* **Neighbor linking:** bidirectional; stored as `std::weak_ptr<GameCell>`.

### 3.2  Core Data Models

```cpp
struct Coordinate { int x, y; /* value-type */ };

enum class Direction : uint8_t { N, NE, E, SE, S, SW, W, NW };
inline Direction rotateClockwise(Direction d);
inline Direction rotateCounterClockwise(Direction d);

enum class CellState : uint8_t { NEUTRAL, PLAYER_1, PLAYER_2, BLOCKED };
enum class Player    : uint8_t { PLAYER_1, PLAYER_2 };

class GameCell {
    Coordinate coordinate_;
    CellState  state_;
    Direction  direction_;
    std::array<std::weak_ptr<GameCell>, 8> neighbors_;
    /* getters/setters elided */
};
```

---

## 4  Game Logic

### 4.1  `IGameLogic` Interface

```cpp
class IGameLogic {
public:
    virtual ~IGameLogic() = default;
    virtual bool        isValidMove(std::shared_ptr<const GameCell>, Player) const = 0;
    virtual GameResult  makeMove   (std::shared_ptr<GameCell>,       Player)      = 0;
    virtual bool        isGameOver () const = 0;
    virtual Player      getCurrentPlayer() const = 0;
    virtual void        switchPlayer()            = 0;
    virtual int         getTurnCount()      const = 0;
    virtual const GameConfig& getConfig()   const = 0;
};
```

### 4.2  State Management

`OctaGameLogic` owns:

```cpp
Player                       currentPlayer_ = Player::PLAYER_1;
int                          turnCount_     = 0;
GameConfig                   config_;
std::optional<GameResult>    gameResult_;
```

### 4.3  Chain-Reaction Algorithm (`makeMove`)

* Follows v3.1 steps (validate, rotate, propagate, etc.).
* **Strong exception safety** provided via **tiered rollback strategy** (see §7).

---

## 5  Game Engine & Flow

### 5.1  `GameEngine`

Orchestrates loop; holds `std::shared_ptr<IGameMap>`, `std::shared_ptr<IGameLogic>`, `std::unique_ptr<MoveProvider>`.

### 5.2  `MoveProvider`

```cpp
class MoveProvider {
public:
    virtual ~MoveProvider() = default;
    virtual std::optional<Coordinate>
        getNextMove(const IGameMap&, Player currentPlayer) = 0;
};
```

---

## 6  Configuration & Win Conditions

### 6.1  `GameConfig`

```cpp
enum class WinCondition { ELIMINATION, TURN_LIMIT_MAJORITY };

enum class SafetyLevel   { VALIDATE_ONLY, LIGHT_UNDO, FULL_ROLLBACK };

struct GameConfig {
    WinCondition winCondition      = WinCondition::ELIMINATION;
    int          turnLimit         = 60;
    bool         stopOnEnemy       = false;
    SafetyLevel  safetyLevel       = SafetyLevel::VALIDATE_ONLY;
};
```

### 6.2  `GameResult`

```cpp
struct GameResult {
    std::optional<Player> winner;   // nullopt => draw
    std::string           reason;   // human-readable
};
```

### 6.3  Win-Condition Rules

* **ELIMINATION** – Game ends when one player controls zero cells (after ≥ 1 turn).
* **TURN\_LIMIT\_MAJORITY** – At `turnCount ≥ turnLimit`, higher cell count wins; tie possible.

---

## 7  Error Handling & Exception Safety

### 7.1  Exception Hierarchy

`OctaCoreException` → `InvalidMoveException`, `LogicInvariantException`, etc.

### 7.2  Tiered Rollback Strategy

| Tier | Safety Level    | Mechanism                                                                      | Default |
| ---- | --------------- | ------------------------------------------------------------------------------ | ------- |
| 0    | `VALIDATE_ONLY` | Simulate on shadow map; commit if valid                                        | **Yes** |
| 1    | `LIGHT_UNDO`    | Record `{coord, oldState, oldDir}` diffs; replay inverse on failure            | No      |
| 2    | `FULL_ROLLBACK` | Command objects with composite transaction; guarantees strong exception safety | No      |

`GameConfig::safetyLevel` selects tier at runtime.

---

## 8  Memory Management Policy

* **Owning pointers:** map stores `std::shared_ptr<GameCell>`.
* **Neighbor refs:** `std::weak_ptr<GameCell>` to break cycles.
* **Value-types:** `Coordinate`, `GameConfig`, `GameResult` passed by value / const-ref.

---

## 9  Performance & Resource Targets

| Metric                      | Target                          | Test Harness     |
| --------------------------- | ------------------------------- | ---------------- |
| Max chain length            | **≤ 1 000 cells**               | Stress test      |
| Median `makeMove()` latency | **≤ 2 ms** on 3 GHz desktop CPU | Google Benchmark |
| 99-th percentile latency    | **≤ 5 ms**                      | Benchmark        |
| Heap per 5×5 game           | **≤ 4 MB**                      | Valgrind massif  |
| Full rebuild time           | **≤ 45 s** on 8-core laptop     | CMake timing     |

**CI fails** if any metric regresses on `main`.

---

## 10  Thread Safety & Concurrency

* Core library **not thread-safe** by default.
* `ThreadSafeGameMap` adapter (compile-time flag `OCTA_THREAD_SAFE`) wraps mutators in `std::scoped_lock`.
* Read-only queries are lock-free.

---

## 11  Serialization & Persistence

| Format           | File/Schema                       | API                                                                 |
| ---------------- | --------------------------------- | ------------------------------------------------------------------- |
| JSON (canonical) | `schemas/octa-state-1.0.json`     | `std::string serialize(StateFormat::JSON) const;`                   |
| FlatBuffers      | `serialization/OctaGameState.fbs` | `std::vector<uint8_t> serialize(StateFormat::FLATBUFFERS) const;`   |
| Deserialize      | —                                 | `static GameState deserialize(std::span<const char>, StateFormat);` |

---

## 12  Debugging, Logging & Diagnostics

* **Log levels:** `TRACE / DEBUG / INFO / WARN / ERROR` — compile-time set via `OCTA_LOG_LEVEL`.
* **Backend:** header-only wrapper for *spdlog*; compiles away if disabled.
* **Godot inspector:** exposes turn, chain length, last move validity.
* **Unity overlay:** editor-only perf stats.

---

## 13  Development Process, Testing & CI

### 13.1  Unit & Integration Tests

* **Google Test** – **≥ 90 %** line coverage (checked by `gcovr` in CI).
* Scenarios: all win conditions, invalid moves, each rollback tier, long-chain perf, leak detection.

### 13.2  Performance & Leak Gates

* **Google Benchmark** auto-publishes JSON results.
* **Valgrind** (Linux) / **Dr Memory** (Windows) – zero “definite” leaks.

### 13.3  CI Pipeline (GitHub Actions)

```yaml
jobs:
  build-test:
    strategy:
      matrix: {os: [ubuntu-latest, windows-latest, macos-latest]}
    steps:
      - uses: actions/checkout@v4
      - run: cmake -B build -DOCTA_THREAD_SAFE=ON -DCMAKE_BUILD_TYPE=Release
      - run: cmake --build build --parallel
      - run: ctest --test-dir build --output-on-failure
      - run: ./build/bench/perf_suite
      - run: valgrind --error-exitcode=1 ./build/test/unit_tests || drmemory -- \
              ./build/test/unit_tests
      - run: gcovr -r . --fail-under-line 90
```

---

## 14  Build, Packaging & Distribution

| Deliverable                           | Notes                                                |
| ------------------------------------- | ---------------------------------------------------- |
| **Static lib** `libocta_core.a`       | default                                              |
| **Shared lib** `libocta_core.so/.dll` | `BUILD_SHARED_LIBS=ON`                               |
| **Godot 4 addon**                     | GDNative C API + `octa.gdextension`                  |
| **Unity package**                     | `OctaCore.unitypackage` (P-Invoke C# wrapper + DLLs) |
| **Conan recipe**                      | `octa_core/1.0.0`                                    |

CMake presets: `dev-debug.json`, `dev-release.json`, `package-unity.json`, `package-godot.json`.

---

## 15  Documentation Standards

* **Doxygen** – config tracked in `docs/Doxyfile`; warnings fail CI.
* Every **public** class/function: `@brief`, param/return, and **one code example**.
* **Tutorials:** “Engine-less Hello World”, “Integrate with Godot”, “Integrate with Unity”.

---

## 16  Implementation Roadmap (8 weeks)

| Phase                                  | Weeks | Milestones                                          |
| -------------------------------------- | ----- | --------------------------------------------------- |
| **P0 Scaffold**                        | 1     | CMake, conan, clang-format, CI skeleton             |
| **P1 Core Logic (Tier 0)**             | 1-3   | Data models, validate-apply algorithm, ≥ 70 % tests |
| **P2 Light Undo & Bench**              | 4-5   | Tier 1 diff buffer, perf targets green              |
| **P3 Full Rollback & ThreadSafe**      | 5-6   | Tier 2 commands, stress tests, leak-free            |
| **P4 Engine Bindings + Serialization** | 6-7   | Godot GDNative, Unity P-Invoke, JSON saves          |
| **P5 Polish & Docs**                   | 8     | Doxygen 100 % public API, tutorials, CI badge       |

---

## 17  Risk & Mitigation

| Risk                      | Likelihood | Impact          | Mitigation                      |
| ------------------------- | ---------- | --------------- | ------------------------------- |
| Command Pattern overhead  | Medium     | Perf regression | Default to Tier 0; perf CI gate |
| Cross-platform DLL issues | Medium     | Build failures  | Conan packages; CI on 3 OSes    |
| Extreme chain length DoS  | Low        | Game freeze     | Hard cap 1 000 cells + watchdog |

---

## 18  Acceptance Criteria

1. All unit, integration, perf & leak tests pass on CI **main**.
2. Coverage ≥ 90 %.
3. Godot sample scene and Unity sample scene compile & run a 5×5 match to completion.
4. Doxygen HTML publishes to GitHub Pages with **0 warnings**.

---

## 19  Next Steps

1. **Clone** repo template; execute **Phase P0** tasks.
2. During **P1**, measure baseline perf to confirm CI thresholds.
3. End of **Week 7:** share engine demo scenes for stakeholder review.
4. Changes after v3.2 require **CR (Change Request)** workflow.

---

*Document ends – v3.2 is now the single source of truth.*
