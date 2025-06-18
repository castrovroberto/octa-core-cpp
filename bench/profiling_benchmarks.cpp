/**
 * @file profiling_benchmarks.cpp
 * @brief Specialized benchmarks for Phase P2.3.2 - Performance Analysis & Profiling
 * 
 * These benchmarks are designed specifically for profiling tools and provide
 * detailed metrics for optimization analysis.
 */

#include <benchmark/benchmark.h>
#include "octa-core/logic/OctaGameLogic.h"
#include "octa-core/map/ArrayGameMap.h"
#include "octa-core/map/GraphGameMap.h"
#include "octa-core/model/GameConfig.h"
#include <memory>
#include <vector>
#include <chrono>
#include <random>

namespace octa_core {
namespace profiling {

// Test configuration for profiling
constexpr int PROFILING_ITERATIONS = 1000;
constexpr int HOTSPOT_ITERATIONS = 10000;

/**
 * @brief Setup helper for consistent game state across benchmarks
 */
class ProfilingSetup {
private:
    std::mt19937 generator_;
    std::uniform_int_distribution<int> coord_dist_;

public:
    ProfilingSetup(int map_radius = 10) 
        : generator_(42), coord_dist_(-map_radius, map_radius) {}
    
    std::unique_ptr<OctaGameLogic> createGameLogic(SafetyLevel safety = SafetyLevel::NONE) {
        GameConfig config;
        config.mapRadius = 10;
        config.safetyLevel = safety;
        
        return std::make_unique<OctaGameLogic>(
            std::make_unique<GraphGameMap>(config.mapRadius), config);
    }
    
    Coordinate getRandomCoordinate() {
        return Coordinate(coord_dist_(generator_), coord_dist_(generator_));
    }
    
    void setupChainReaction(OctaGameLogic& logic, int chain_length) {
        // Setup a predictable chain reaction
        for (int i = 0; i < chain_length; ++i) {
            Coordinate coord(i, 0);
            auto cell = logic.getGameMap()->getCell(coord);
            if (cell) {
                // Set cells to near-explosion state
                cell->setOrbCount(cell->getCapacity() - 1);
            }
        }
    }
};

/**
 * @brief CPU Hotspot Analysis - Focus on makeMove() hot paths
 */
static void BM_Profiling_MakeMoveHotspot(benchmark::State& state) {
    ProfilingSetup setup;
    auto logic = setup.createGameLogic();
    
    // Pre-setup some cells for consistent workload
    setup.setupChainReaction(*logic, 5);
    
    std::vector<Coordinate> test_coords;
    for (int i = 0; i < 100; ++i) {
        test_coords.push_back(setup.getRandomCoordinate());
    }
    
    size_t coord_idx = 0;
    
    for (auto _ : state) {
        // This will hit the hot path repeatedly
        auto coord = test_coords[coord_idx % test_coords.size()];
        
        // Measure hot path execution
        auto start = std::chrono::high_resolution_clock::now();
        logic->makeMove(coord);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        state.SetIterationTime(duration.count() / 1e9);
        
        coord_idx++;
    }
    
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Profiling_MakeMoveHotspot)->UseManualTime()->Iterations(HOTSPOT_ITERATIONS);

/**
 * @brief Memory Allocation Pattern Analysis
 */
static void BM_Profiling_AllocationPattern(benchmark::State& state) {
    ProfilingSetup setup;
    
    for (auto _ : state) {
        // Create and destroy game logic to measure allocation patterns
        auto logic = setup.createGameLogic();
        setup.setupChainReaction(*logic, 3);
        
        // Force some allocations through gameplay
        for (int i = 0; i < 10; ++i) {
            logic->makeMove(setup.getRandomCoordinate());
        }
        
        // Explicit scope end to trigger deallocations
    }
    
    state.SetItemsProcessed(state.iterations() * 10);
}
BENCHMARK(BM_Profiling_AllocationPattern)->Iterations(1000);

/**
 * @brief Chain Propagation Deep Analysis
 */
static void BM_Profiling_ChainPropagation(benchmark::State& state) {
    ProfilingSetup setup;
    auto logic = setup.createGameLogic();
    
    const int chain_length = state.range(0);
    
    for (auto _ : state) {
        state.PauseTiming();
        
        // Reset and setup chain
        logic = setup.createGameLogic();
        setup.setupChainReaction(*logic, chain_length);
        
        state.ResumeTiming();
        
        // Trigger the chain reaction
        logic->makeMove(Coordinate(0, 0));
    }
    
    state.SetComplexityN(chain_length);
    state.SetItemsProcessed(state.iterations() * chain_length);
}
BENCHMARK(BM_Profiling_ChainPropagation)
    ->RangeMultiplier(2)
    ->Range(1, 32)
    ->Complexity(benchmark::oN);

/**
 * @brief Memory Access Pattern Analysis - Cache Performance
 */
static void BM_Profiling_CacheAccessPattern(benchmark::State& state) {
    ProfilingSetup setup;
    auto logic = setup.createGameLogic();
    
    const int stride = state.range(0);
    
    // Pre-populate map with data
    for (int x = -20; x <= 20; x += stride) {
        for (int y = -20; y <= 20; y += stride) {
            auto cell = logic->getGameMap()->getCell(Coordinate(x, y));
            if (cell) {
                cell->setOrbCount(1);
            }
        }
    }
    
    std::vector<Coordinate> access_pattern;
    for (int x = -20; x <= 20; x += stride) {
        for (int y = -20; y <= 20; y += stride) {
            access_pattern.emplace_back(x, y);
        }
    }
    
    size_t access_idx = 0;
    
    for (auto _ : state) {
        // Access cells in the pattern
        auto coord = access_pattern[access_idx % access_pattern.size()];
        auto cell = logic->getGameMap()->getCell(coord);
        
        if (cell) {
            benchmark::DoNotOptimize(cell->getOrbCount());
        }
        
        access_idx++;
    }
    
    state.SetItemsProcessed(state.iterations());
    state.counters["stride"] = stride;
    state.counters["cache_misses_per_access"] = 
        (stride > 1) ? static_cast<double>(stride) / 8.0 : 0.1;
}
BENCHMARK(BM_Profiling_CacheAccessPattern)
    ->Arg(1)   // Sequential access (cache-friendly)
    ->Arg(2)   // Moderate stride
    ->Arg(4)   // Larger stride
    ->Arg(8)   // Cache line boundary
    ->Arg(16); // Cache-unfriendly

/**
 * @brief Function Call Overhead Analysis
 */
static void BM_Profiling_FunctionCallOverhead(benchmark::State& state) {
    ProfilingSetup setup;
    auto logic = setup.createGameLogic();
    
    Coordinate test_coord(0, 0);
    
    for (auto _ : state) {
        // Measure the overhead of repeated function calls
        auto map = logic->getGameMap();
        auto cell = map->getCell(test_coord);
        
        if (cell) {
            auto count = cell->getOrbCount();
            auto capacity = cell->getCapacity();
            auto neighbors = map->getNeighbors(test_coord);
            
            benchmark::DoNotOptimize(count);
            benchmark::DoNotOptimize(capacity);
            benchmark::DoNotOptimize(neighbors);
        }
    }
    
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Profiling_FunctionCallOverhead)->Iterations(HOTSPOT_ITERATIONS);

/**
 * @brief Safety Level Overhead Deep Analysis
 */
static void BM_Profiling_SafetyOverheadDeep(benchmark::State& state) {
    ProfilingSetup setup;
    
    SafetyLevel safety_level = static_cast<SafetyLevel>(state.range(0));
    auto logic = setup.createGameLogic(safety_level);
    
    setup.setupChainReaction(*logic, 3);
    
    std::vector<Coordinate> test_coords;
    for (int i = 0; i < 50; ++i) {
        test_coords.push_back(setup.getRandomCoordinate());
    }
    
    size_t coord_idx = 0;
    
    for (auto _ : state) {
        auto coord = test_coords[coord_idx % test_coords.size()];
        logic->makeMove(coord);
        coord_idx++;
    }
    
    state.SetItemsProcessed(state.iterations());
    state.counters["safety_level"] = static_cast<int>(safety_level);
}
BENCHMARK(BM_Profiling_SafetyOverheadDeep)
    ->Arg(static_cast<int>(SafetyLevel::NONE))
    ->Arg(static_cast<int>(SafetyLevel::LIGHT_UNDO));

/**
 * @brief Branch Prediction Analysis
 */
static void BM_Profiling_BranchPrediction(benchmark::State& state) {
    ProfilingSetup setup;
    auto logic = setup.createGameLogic();
    
    const bool predictable = state.range(0) == 1;
    
    std::vector<Coordinate> coords;
    if (predictable) {
        // Predictable pattern - same cells
        for (int i = 0; i < 100; ++i) {
            coords.emplace_back(0, 0);
        }
    } else {
        // Unpredictable pattern - random cells
        for (int i = 0; i < 100; ++i) {
            coords.push_back(setup.getRandomCoordinate());
        }
    }
    
    size_t coord_idx = 0;
    
    for (auto _ : state) {
        auto coord = coords[coord_idx % coords.size()];
        
        // This will cause different branch patterns
        auto cell = logic->getGameMap()->getCell(coord);
        if (cell) {
            if (cell->getOrbCount() > 0) {
                benchmark::DoNotOptimize(cell->getOrbCount());
            } else {
                benchmark::DoNotOptimize(cell->getCapacity());
            }
        }
        
        coord_idx++;
    }
    
    state.SetItemsProcessed(state.iterations());
    state.counters["predictable"] = predictable ? 1 : 0;
}
BENCHMARK(BM_Profiling_BranchPrediction)
    ->Arg(1)  // Predictable branches
    ->Arg(0); // Unpredictable branches

/**
 * @brief Container Operation Profiling
 */
static void BM_Profiling_ContainerOperations(benchmark::State& state) {
    ProfilingSetup setup;
    auto logic = setup.createGameLogic();
    
    Coordinate center(0, 0);
    
    for (auto _ : state) {
        // Profile different container operations
        auto neighbors = logic->getGameMap()->getNeighbors(center);
        
        // Vector operations
        std::vector<std::shared_ptr<GameCell>> temp_cells;
        for (const auto& coord : neighbors) {
            auto cell = logic->getGameMap()->getCell(coord);
            if (cell) {
                temp_cells.push_back(cell);
            }
        }
        
        // Sort and access patterns
        std::sort(temp_cells.begin(), temp_cells.end(),
                  [](const auto& a, const auto& b) {
                      return a->getOrbCount() < b->getOrbCount();
                  });
        
        for (const auto& cell : temp_cells) {
            benchmark::DoNotOptimize(cell->getOrbCount());
        }
    }
    
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Profiling_ContainerOperations)->Iterations(PROFILING_ITERATIONS);

/**
 * @brief Thread Safety Analysis (if applicable)
 */
static void BM_Profiling_ThreadSafety(benchmark::State& state) {
    ProfilingSetup setup;
    auto logic = setup.createGameLogic();
    
    // Multiple threads accessing the same game logic
    // Note: This is primarily for measuring synchronization overhead
    for (auto _ : state) {
        auto coord = setup.getRandomCoordinate();
        auto cell = logic->getGameMap()->getCell(coord);
        
        if (cell) {
            // Read operations that might require synchronization
            auto count = cell->getOrbCount();
            auto capacity = cell->getCapacity();
            
            benchmark::DoNotOptimize(count);
            benchmark::DoNotOptimize(capacity);
        }
    }
    
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Profiling_ThreadSafety)
    ->Threads(1)
    ->Threads(2)
    ->Threads(4);

} // namespace profiling
} // namespace octa_core 