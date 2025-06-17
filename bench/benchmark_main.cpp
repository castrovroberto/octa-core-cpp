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
#include "octa-core/map/GraphGameMap.h"
#include "octa-core/logic/OctaGameLogic.h"
#include "octa-core/model/GameConfig.h"
#include "octa-core/core/Direction.h"

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
    auto gameMap = std::make_shared<GraphGameMap>(2); // 5x5 map
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
    auto gameMap = std::make_shared<GraphGameMap>(2); // 5x5 map
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
    auto gameMap = std::make_shared<GraphGameMap>(2); // 5x5 map
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
    auto gameMap = std::make_shared<GraphGameMap>(2); // 5x5 map
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
    auto gameMap = std::make_shared<GraphGameMap>(1); // 3x3 map for faster testing
    GameConfig config(WinCondition::TURN_LIMIT_MAJORITY, 10, false, SafetyLevel::VALIDATE_ONLY);
    
    std::vector<Coordinate> moveSequence = {
        Coordinate(0, 0), Coordinate(1, 0), Coordinate(-1, 0),
        Coordinate(0, 1), Coordinate(0, -1), Coordinate(1, 1),
        Coordinate(-1, -1), Coordinate(-1, 1), Coordinate(1, -1)
    };
    
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
BENCHMARK(BM_GameMap_Creation)->Range(1, 4); // Test map sizes 1-4 (3x3 to 9x9)

/**
 * @brief Benchmark cell access performance
 */
static void BM_GameMap_CellAccess(benchmark::State& state) {
    auto gameMap = std::make_shared<GraphGameMap>(2); // 5x5 map
    
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
        
        dir = clockwise; // Rotate for next iteration
    }
}
BENCHMARK(BM_Direction_Operations);

/**
 * @brief Benchmark runner main function
 * 
 * This function initializes Google Benchmark and runs all benchmarks.
 * Individual performance benchmarks will be added in Phase P2.
 */
BENCHMARK_MAIN(); 