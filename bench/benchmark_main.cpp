/**
 * @file benchmark_main.cpp
 * @brief Basic benchmark runner for Octa-Core C++ library
 *
 * This file provides the main entry point for Google Benchmark.
 * Individual benchmark files will be added in Phase P2.
 *
 * Phase P0: Basic benchmark infrastructure
 */

#include <benchmark/benchmark.h>

// Include game logic components for P1.4 benchmarks
#include "octa-core/core/Direction.h"
#include "octa-core/logic/OctaGameLogic.h"
#include "octa-core/map/GraphGameMap.h"
#include "octa-core/model/GameConfig.h"

/**
 * @brief Basic placeholder benchmark to verify benchmark infrastructure works
 */
static void BM_PhaseP0_Infrastructure(benchmark::State& state) {
    // Basic benchmark to verify Google Benchmark is working
    for (auto _ : state) {
        // Simple computation to benchmark
        volatile int result = 0;
        for (int i = 0; i < 1000; ++i) {
            result += i;
        }
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_PhaseP0_Infrastructure);

// ============================================================================
// PHASE P1.4 GAME LOGIC BENCHMARKS
// ============================================================================

/**
 * @brief Benchmark makeMove performance for single cell moves
 */
static void BM_GameLogic_MakeMove_SingleCell(benchmark::State& state) {
    auto gameMap = std::make_shared<GraphGameMap>(2);  // 5x5 map
    GameConfig config(WinCondition::ELIMINATION, 100, false, SafetyLevel::VALIDATE_ONLY);
    OctaGameLogic logic(gameMap, config);

    auto centerCell = gameMap->at(Coordinate(0, 0));

    for (auto _ : state) {
        // Reset for each iteration
        logic.resetGame();

        // Benchmark single move execution
        auto result = logic.makeMove(centerCell, Player::PLAYER_1);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_GameLogic_MakeMove_SingleCell);

/**
 * @brief Benchmark makeMove performance for chain reactions
 */
static void BM_GameLogic_MakeMove_ChainReaction(benchmark::State& state) {
    auto gameMap = std::make_shared<GraphGameMap>(2);  // 5x5 map
    GameConfig config(WinCondition::ELIMINATION, 100, false, SafetyLevel::VALIDATE_ONLY);
    OctaGameLogic logic(gameMap, config);

    for (auto _ : state) {
        // Reset and setup chain scenario
        logic.resetGame();

        // Setup a chain: center -> right -> top
        auto centerCell = gameMap->at(Coordinate(0, 0));
        auto rightCell = gameMap->at(Coordinate(1, 0));
        auto topCell = gameMap->at(Coordinate(0, 1));

        centerCell->setState(CellState::PLAYER_1);
        centerCell->setDirection(Direction::E);

        // Benchmark chain reaction move
        auto result = logic.makeMove(centerCell, Player::PLAYER_1);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_GameLogic_MakeMove_ChainReaction);

/**
 * @brief Benchmark isValidMove performance
 */
static void BM_GameLogic_IsValidMove(benchmark::State& state) {
    auto gameMap = std::make_shared<GraphGameMap>(2);  // 5x5 map
    GameConfig config(WinCondition::ELIMINATION, 100, false, SafetyLevel::VALIDATE_ONLY);
    OctaGameLogic logic(gameMap, config);

    auto centerCell = gameMap->at(Coordinate(0, 0));

    for (auto _ : state) {
        bool result = logic.isValidMove(centerCell, Player::PLAYER_1);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_GameLogic_IsValidMove);

/**
 * @brief Benchmark game state queries
 */
static void BM_GameLogic_StateQueries(benchmark::State& state) {
    auto gameMap = std::make_shared<GraphGameMap>(2);  // 5x5 map
    GameConfig config(WinCondition::ELIMINATION, 100, false, SafetyLevel::VALIDATE_ONLY);
    OctaGameLogic logic(gameMap, config);

    for (auto _ : state) {
        auto currentPlayer = logic.getCurrentPlayer();
        auto turnCount = logic.getTurnCount();
        auto isGameOver = logic.isGameOver();

        benchmark::DoNotOptimize(currentPlayer);
        benchmark::DoNotOptimize(turnCount);
        benchmark::DoNotOptimize(isGameOver);
    }
}
BENCHMARK(BM_GameLogic_StateQueries);

/**
 * @brief Benchmark complete game scenario performance
 */
static void BM_GameLogic_CompleteGame(benchmark::State& state) {
    auto gameMap = std::make_shared<GraphGameMap>(1);  // 3x3 map for faster testing
    GameConfig config(WinCondition::TURN_LIMIT_MAJORITY, 10, false, SafetyLevel::VALIDATE_ONLY);

    std::vector<Coordinate> moveSequence = {
        Coordinate(0, 0),   Coordinate(1, 0),  Coordinate(-1, 0),
        Coordinate(0, 1),   Coordinate(0, -1), Coordinate(1, 1),
        Coordinate(-1, -1), Coordinate(-1, 1), Coordinate(1, -1)};

    for (auto _ : state) {
        OctaGameLogic logic(gameMap, config);

        // Execute move sequence
        for (const auto& coord : moveSequence) {
            if (logic.isGameOver()) break;

            auto cell = gameMap->at(coord);
            if (cell && logic.isValidMove(cell, logic.getCurrentPlayer())) {
                auto result = logic.makeMove(cell, logic.getCurrentPlayer());
                benchmark::DoNotOptimize(result);
            }
        }
    }
}
BENCHMARK(BM_GameLogic_CompleteGame);

/**
 * @brief Benchmark map creation and initialization
 */
static void BM_GameMap_Creation(benchmark::State& state) {
    const int mapSize = state.range(0);

    for (auto _ : state) {
        auto gameMap = std::make_shared<GraphGameMap>(mapSize);
        benchmark::DoNotOptimize(gameMap);
    }
}
BENCHMARK(BM_GameMap_Creation)->Range(1, 4);  // Test map sizes 1-4 (3x3 to 9x9)

/**
 * @brief Benchmark cell access performance
 */
static void BM_GameMap_CellAccess(benchmark::State& state) {
    auto gameMap = std::make_shared<GraphGameMap>(2);  // 5x5 map

    for (auto _ : state) {
        // Access all cells in the map
        for (int x = -2; x <= 2; ++x) {
            for (int y = -2; y <= 2; ++y) {
                auto cell = gameMap->at(Coordinate(x, y));
                benchmark::DoNotOptimize(cell);
            }
        }
    }
}
BENCHMARK(BM_GameMap_CellAccess);

/**
 * @brief Benchmark direction operations
 */
static void BM_Direction_Operations(benchmark::State& state) {
    Direction dir = Direction::N;

    for (auto _ : state) {
        auto clockwise = rotateClockwise(dir);
        auto counterclockwise = rotateCounterClockwise(dir);
        auto opposite = getOpposite(dir);

        benchmark::DoNotOptimize(clockwise);
        benchmark::DoNotOptimize(counterclockwise);
        benchmark::DoNotOptimize(opposite);

        dir = clockwise;  // Rotate for next iteration
    }
}
BENCHMARK(BM_Direction_Operations);

// ============================================================================
// PHASE P2.2 CORE PERFORMANCE BENCHMARKS
// ============================================================================

/**
 * @brief Helper function to create a chain reaction setup of specified length
 * @param gameMap The game map to set up
 * @param chainLength Target chain length
 * @return Starting cell for the chain reaction
 */
static std::shared_ptr<GameCell> setupChainReaction(std::shared_ptr<GraphGameMap> gameMap,
                                                    int chainLength) {
    // Create a controlled chain setup
    // Start from center and work outward in a predictable pattern
    std::vector<Coordinate> coords = {
        Coordinate(0, 0),    // Center
        Coordinate(1, 0),    // Right
        Coordinate(0, 1),    // Up
        Coordinate(-1, 0),   // Left
        Coordinate(0, -1),   // Down
        Coordinate(1, 1),    // NE
        Coordinate(-1, 1),   // NW
        Coordinate(-1, -1),  // SW
        Coordinate(1, -1),   // SE
    };

    // Set up cells to create a chain of specified length
    for (int i = 0; i < std::min(chainLength, (int)coords.size()); ++i) {
        auto cell = gameMap->at(coords[i]);
        if (cell) {
            cell->setState(CellState::PLAYER_1);
            // Set value to just below explosion threshold to ensure chain propagation
            int neighborCount = cell->getValidNeighborCount();
            cell->setValue(neighborCount - 1);  // Will explode when incremented
        }
    }

    return gameMap->at(Coordinate(0, 0));  // Return starting cell
}

/**
 * @brief Benchmark short chain reactions (< 10 cells)
 */
static void BM_MakeMove_ShortChain(benchmark::State& state) {
    auto gameMap = std::make_shared<GraphGameMap>(2);  // 5x5 map
    GameConfig config(WinCondition::ELIMINATION, 100, false, SafetyLevel::VALIDATE_ONLY);
    OctaGameLogic logic(gameMap, config);

    for (auto _ : state) {
        logic.resetGame();
        auto startCell = setupChainReaction(gameMap, 5);  // 5-cell chain

        auto result = logic.makeMove(startCell, Player::PLAYER_1);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_MakeMove_ShortChain);

/**
 * @brief Benchmark medium chain reactions (20-50 cells)
 */
static void BM_MakeMove_MediumChain(benchmark::State& state) {
    auto gameMap = std::make_shared<GraphGameMap>(4);  // 9x9 map for medium chains
    GameConfig config(WinCondition::ELIMINATION, 100, false, SafetyLevel::VALIDATE_ONLY);
    OctaGameLogic logic(gameMap, config);

    for (auto _ : state) {
        logic.resetGame();

        // Create medium chain setup - more complex pattern
        for (int x = -2; x <= 2; ++x) {
            for (int y = -2; y <= 2; ++y) {
                auto cell = gameMap->at(Coordinate(x, y));
                if (cell && (x + y) % 2 == 0) {  // Checkerboard pattern
                    cell->setState(CellState::PLAYER_1);
                    int neighborCount = cell->getValidNeighborCount();
                    cell->setValue(neighborCount - 1);
                }
            }
        }

        auto startCell = gameMap->at(Coordinate(0, 0));
        auto result = logic.makeMove(startCell, Player::PLAYER_1);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_MakeMove_MediumChain);

/**
 * @brief Benchmark long chain reactions (100+ cells)
 */
static void BM_MakeMove_LongChain(benchmark::State& state) {
    auto gameMap = std::make_shared<GraphGameMap>(6);  // 13x13 map for long chains
    GameConfig config(WinCondition::ELIMINATION, 1000, false, SafetyLevel::VALIDATE_ONLY);
    OctaGameLogic logic(gameMap, config);

    for (auto _ : state) {
        logic.resetGame();

        // Create long chain setup - spiral pattern from center outward
        for (int x = -6; x <= 6; ++x) {
            for (int y = -6; y <= 6; ++y) {
                auto cell = gameMap->at(Coordinate(x, y));
                if (cell && (abs(x) + abs(y)) % 3 == 0) {  // Sparse pattern for long chains
                    cell->setState(CellState::PLAYER_1);
                    int neighborCount = cell->getValidNeighborCount();
                    cell->setValue(neighborCount - 1);
                }
            }
        }

        auto startCell = gameMap->at(Coordinate(0, 0));
        auto result = logic.makeMove(startCell, Player::PLAYER_1);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_MakeMove_LongChain);

/**
 * @brief Benchmark moves with no chain reaction (immediate termination)
 */
static void BM_MakeMove_NoChain(benchmark::State& state) {
    auto gameMap = std::make_shared<GraphGameMap>(2);  // 5x5 map
    GameConfig config(WinCondition::ELIMINATION, 100, false, SafetyLevel::VALIDATE_ONLY);
    OctaGameLogic logic(gameMap, config);

    auto centerCell = gameMap->at(Coordinate(0, 0));

    for (auto _ : state) {
        logic.resetGame();

        // Set up cell that won't explode (low value)
        centerCell->setValue(1);  // Well below explosion threshold

        auto result = logic.makeMove(centerCell, Player::PLAYER_1);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_MakeMove_NoChain);

/**
 * @brief Benchmark complex branching scenarios (multiple directions)
 */
static void BM_MakeMove_ComplexBranching(benchmark::State& state) {
    auto gameMap = std::make_shared<GraphGameMap>(3);  // 7x7 map
    GameConfig config(WinCondition::ELIMINATION, 100, false, SafetyLevel::VALIDATE_ONLY);
    OctaGameLogic logic(gameMap, config);

    for (auto _ : state) {
        logic.resetGame();

        // Create star pattern for complex branching
        std::vector<Coordinate> starPattern = {
            Coordinate(0, 0),    // Center
            Coordinate(2, 0),    // East arm
            Coordinate(0, 2),    // North arm
            Coordinate(-2, 0),   // West arm
            Coordinate(0, -2),   // South arm
            Coordinate(1, 1),    // NE arm
            Coordinate(-1, 1),   // NW arm
            Coordinate(-1, -1),  // SW arm
            Coordinate(1, -1),   // SE arm
        };

        for (const auto& coord : starPattern) {
            auto cell = gameMap->at(coord);
            if (cell) {
                cell->setState(CellState::PLAYER_1);
                int neighborCount = cell->getValidNeighborCount();
                cell->setValue(neighborCount - 1);
            }
        }

        auto startCell = gameMap->at(Coordinate(0, 0));
        auto result = logic.makeMove(startCell, Player::PLAYER_1);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_MakeMove_ComplexBranching);

/**
 * @brief Benchmark moves with LIGHT_UNDO safety overhead
 */
static void BM_MakeMove_WithLightUndo(benchmark::State& state) {
    auto gameMap = std::make_shared<GraphGameMap>(2);  // 5x5 map
    GameConfig config(WinCondition::ELIMINATION, 100, false,
                      SafetyLevel::LIGHT_UNDO);  // Enable LIGHT_UNDO
    OctaGameLogic logic(gameMap, config);

    for (auto _ : state) {
        logic.resetGame();
        auto startCell = setupChainReaction(gameMap, 5);  // 5-cell chain with undo logging

        auto result = logic.makeMove(startCell, Player::PLAYER_1);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_MakeMove_WithLightUndo);

/**
 * @brief Benchmark maximum chain length stress test
 */
static void BM_MakeMove_MaxChainLength(benchmark::State& state) {
    auto gameMap = std::make_shared<GraphGameMap>(8);  // 17x17 map for maximum chains
    GameConfig config(WinCondition::ELIMINATION, 2000, false, SafetyLevel::VALIDATE_ONLY);
    OctaGameLogic logic(gameMap, config);

    for (auto _ : state) {
        logic.resetGame();

        // Create maximum density chain setup
        for (int x = -8; x <= 8; ++x) {
            for (int y = -8; y <= 8; ++y) {
                auto cell = gameMap->at(Coordinate(x, y));
                if (cell && (x * x + y * y) % 7 == 0) {  // Radial sparse pattern
                    cell->setState(CellState::PLAYER_1);
                    int neighborCount = cell->getValidNeighborCount();
                    cell->setValue(neighborCount - 1);
                }
            }
        }

        auto startCell = gameMap->at(Coordinate(0, 0));
        auto result = logic.makeMove(startCell, Player::PLAYER_1);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_MakeMove_MaxChainLength);

/**
 * @brief Benchmark large map size stress test
 */
static void BM_MakeMove_LargeMapStress(benchmark::State& state) {
    const int mapSize = state.range(0);
    auto gameMap = std::make_shared<GraphGameMap>(mapSize);
    GameConfig config(WinCondition::ELIMINATION, 100, false, SafetyLevel::VALIDATE_ONLY);
    OctaGameLogic logic(gameMap, config);

    for (auto _ : state) {
        logic.resetGame();

        // Simple move on large map to test map size overhead
        auto centerCell = gameMap->at(Coordinate(0, 0));
        if (centerCell) {
            auto result = logic.makeMove(centerCell, Player::PLAYER_1);
            benchmark::DoNotOptimize(result);
        }
    }
}
BENCHMARK(BM_MakeMove_LargeMapStress)->Range(2, 10);  // Test map sizes from 5x5 to 21x21

/**
 * @brief Benchmark runner main function
 *
 * This function initializes Google Benchmark and runs all benchmarks.
 * Individual performance benchmarks will be added in Phase P2.
 */
BENCHMARK_MAIN();