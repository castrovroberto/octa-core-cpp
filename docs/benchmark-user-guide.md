# Benchmark User Guide - Octa-Core C++

**Version**: 1.0  
**Last Updated**: 2025-06-18  
**Status**: ✅ COMPLETE

---

## 📋 **Table of Contents**

1. [Quick Start](#quick-start)
2. [Benchmark Categories](#benchmark-categories)
3. [Running Benchmarks](#running-benchmarks)
4. [Interpreting Results](#interpreting-results)
5. [Advanced Usage](#advanced-usage)
6. [Extending Benchmarks](#extending-benchmarks)
7. [Troubleshooting](#troubleshooting)
8. [Performance Targets](#performance-targets)

---

## 🚀 **Quick Start**

### **Running All Benchmarks**
```bash
# Quick benchmark run
./scripts/quick.sh bench

# Detailed benchmark with statistics
./build/build/Release/perf_suite \
  --benchmark_repetitions=3 \
  --benchmark_report_aggregates_only=true
```

### **Sample Output**
```
BM_MakeMove_ShortChain         9.83 us         9.82 us        71361
BM_SafetyLevel_Comparison/0   14.77 us        14.75 us        47062 VALIDATE_ONLY
BM_SafetyLevel_Comparison/1   14.81 us        14.81 us        47321 LIGHT_UNDO
```

---

## 📊 **Benchmark Categories**

### **Phase P2.2.2 - Core Performance Benchmarks**

#### **1. Chain Length Variations**
- **`BM_MakeMove_ShortChain`**: < 10 cells affected
- **`BM_MakeMove_MediumChain`**: 20-50 cells affected
- **`BM_MakeMove_LongChain`**: 100-1000 cells affected

**Purpose**: Measure performance scaling with chain reaction complexity

#### **2. Special Scenarios**
- **`BM_MakeMove_NoChain`**: Immediate termination scenarios
- **`BM_MakeMove_ComplexBranching`**: Multi-directional propagation
- **`BM_MakeMove_WithLightUndo`**: Safety system overhead

**Purpose**: Validate performance in edge cases and safety scenarios

#### **3. Stress Testing**
- **`BM_MakeMove_MaxChainLength`**: 1000 cell chain limit
- **`BM_MakeMove_LargeMapStress`**: Various map sizes (radius 2-10)

**Purpose**: Confirm performance under maximum load conditions

### **Phase P2.2.3 - Advanced Benchmark Features**

#### **4. Memory Tracking**
- **`BM_MakeMove_MemoryTracking`**: Allocation patterns during moves
- **Custom Metrics**: `bytes_per_second`, `items_per_second`

**Purpose**: Analyze memory usage and throughput characteristics

#### **5. Scalability Analysis**
- **`BM_Scalability_MapSize`**: Performance vs map radius
- **Complexity Analysis**: Automatic O(N) calculation

**Purpose**: Validate scaling behavior and algorithmic complexity

#### **6. Safety Level Comparison**
- **`BM_SafetyLevel_Comparison/0`**: VALIDATE_ONLY mode
- **`BM_SafetyLevel_Comparison/1`**: LIGHT_UNDO mode

**Purpose**: Measure safety system overhead

#### **7. Cache Performance**
- **`BM_CachePerformance_AccessPattern`**: Memory access patterns
- **Stride Testing**: Different memory access patterns

**Purpose**: Optimize data structure layout and access patterns

#### **8. Resource Utilization**
- **`BM_ResourceUtilization_Comprehensive`**: Multi-metric analysis
- **Custom Counters**: `StateChanges`, `AvgChangesPerMove`

**Purpose**: Comprehensive resource consumption analysis

#### **9. Throughput & Allocation**
- **`BM_Throughput_MovesPerSecond`**: Moves per second measurement
- **`BM_MemoryAllocation_Patterns`**: Allocation strategy comparison

**Purpose**: Optimize high-frequency operations and memory management

---

## 🔧 **Running Benchmarks**

### **Basic Commands**

#### **Full Suite**
```bash
# Using convenience script
./scripts/quick.sh bench

# Direct execution
./build/build/Release/perf_suite
```

#### **Filtered Execution**
```bash
# Run only chain length benchmarks
./build/build/Release/perf_suite --benchmark_filter=".*Chain.*"

# Run only safety comparison benchmarks
./build/build/Release/perf_suite --benchmark_filter=".*Safety.*"

# Run only memory-related benchmarks
./build/build/Release/perf_suite --benchmark_filter=".*Memory.*"
```

### **Statistical Analysis**

#### **Reproducibility Testing**
```bash
# Run with multiple repetitions
./build/build/Release/perf_suite \
  --benchmark_repetitions=5 \
  --benchmark_report_aggregates_only=true
```

#### **JSON Output for Analysis**
```bash
# Generate JSON report
./build/build/Release/perf_suite \
  --benchmark_format=json \
  --benchmark_out=performance_report.json
```

#### **Detailed Statistics**
```bash
# Show all statistics (mean, median, stddev, CV)
./build/build/Release/perf_suite \
  --benchmark_repetitions=3 \
  --benchmark_display_aggregates_only=false
```

### **Performance Profiling**

#### **Time-Based Filtering**
```bash
# Run only fast benchmarks (< 1ms)
./build/build/Release/perf_suite \
  --benchmark_min_time=0.001

# Run for specific duration
./build/build/Release/perf_suite \
  --benchmark_time_unit=us
```

---

## 📈 **Interpreting Results**

### **Understanding Output Format**

#### **Basic Output**
```
Benchmark                    Time             CPU   Iterations
BM_MakeMove_ShortChain     9.83 us         9.82 us        71361
```

- **Benchmark**: Test name
- **Time**: Wall clock time (real time)
- **CPU**: CPU time (user + system)
- **Iterations**: Number of runs executed

#### **Advanced Output with Custom Metrics**
```
BM_MakeMove_MemoryTracking/64   14.8 us   14.8 us   47539 
  bytes_per_second=595.06Mi/s 
  items_per_second=4.3331M/s 
  memory_tracking
```

- **bytes_per_second**: Memory throughput
- **items_per_second**: Processing throughput
- **Label**: Custom identifier for the benchmark

### **Statistical Interpretation**

#### **Aggregate Statistics**
```
BM_MakeMove_ShortChain_mean      9.83 us   9.82 us    71361
BM_MakeMove_ShortChain_median    9.82 us   9.81 us    71361
BM_MakeMove_ShortChain_stddev    0.05 us   0.05 us    71361
BM_MakeMove_ShortChain_cv        0.51 %    0.51 %     71361
```

- **mean**: Average performance
- **median**: Middle value (less affected by outliers)
- **stddev**: Standard deviation (variability)
- **cv**: Coefficient of variation (% variability)

#### **Performance Quality Indicators**

| **CV Range** | **Quality** | **Interpretation** |
|--------------|-------------|-------------------|
| < 1% | ✅ Excellent | Very stable, reliable measurements |
| 1-3% | ✅ Good | Acceptable variability |
| 3-10% | ⚠️ Fair | Some variability, investigate |
| > 10% | ❌ Poor | High variability, unreliable |

### **Scalability Analysis**

#### **Complexity Analysis Output**
```
BM_Scalability_MapSize_BigO      5833.9 N       5733.0 N     
BM_Scalability_MapSize_RMS          16 %           16 %      
```

- **BigO**: Computational complexity coefficient
- **RMS**: Root Mean Square error (fit quality)
- **N**: Linear complexity confirmed

---

## 🔬 **Advanced Usage**

### **Custom Benchmark Filtering**

#### **Regular Expression Filtering**
```bash
# Match specific patterns
./build/build/Release/perf_suite --benchmark_filter="BM_MakeMove_.*Chain"

# Exclude specific benchmarks
./build/build/Release/perf_suite --benchmark_filter="-.*Stress.*"

# Complex regex patterns
./build/build/Release/perf_suite --benchmark_filter="(Memory|Cache).*"
```

### **Performance Regression Detection**

#### **Baseline Comparison**
```bash
# 1. Establish baseline
./build/build/Release/perf_suite \
  --benchmark_format=json \
  --benchmark_out=baseline.json

# 2. Run current tests
./build/build/Release/perf_suite \
  --benchmark_format=json \
  --benchmark_out=current.json

# 3. Compare (requires Google benchmark tools)
# compare.py baseline.json current.json
```

### **Environment Optimization**

#### **System Configuration**
```bash
# Set CPU governor to performance
sudo cpupower frequency-set --governor performance

# Disable CPU frequency scaling
echo 1 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo

# Minimize background processes
systemctl stop bluetooth
systemctl stop wpa_supplicant
```

#### **Build Configuration**
```bash
# Ensure Release build
cmake --preset conan-release -DBUILD_BENCHMARKS=ON
cmake --build --preset conan-release --target perf_suite

# Verify optimization flags
cmake --build --preset conan-release --verbose
```

---

## 🛠 **Extending Benchmarks**

### **Adding New Benchmarks**

#### **Basic Benchmark Structure**
```cpp
// Add to bench/benchmark_main.cpp

/**
 * @brief Your benchmark description
 * @param state Benchmark state parameter
 */
static void BM_YourBenchmark(benchmark::State& state) {
    // Setup code (not measured)
    auto gameMap = std::make_shared<GraphGameMap>(3);
    GameConfig config;
    OctaGameLogic logic(gameMap, config);
    
    for (auto _ : state) {
        state.PauseTiming();
        // Reset state for each iteration
        logic.resetGame();
        auto startCell = setupYourScenario(gameMap);
        state.ResumeTiming();
        
        // Measured code
        logic.makeMove(startCell, Player::PLAYER_1);
    }
    
    // Optional: Custom metrics
    state.SetItemsProcessed(state.iterations());
    state.SetLabel("your_label");
}
BENCHMARK(BM_YourBenchmark)->Unit(benchmark::kMicrosecond);
```

#### **Parameterized Benchmarks**
```cpp
static void BM_ParameterizedBenchmark(benchmark::State& state) {
    const int parameter = static_cast<int>(state.range(0));
    
    for (auto _ : state) {
        // Use parameter in your benchmark
        performOperationWithSize(parameter);
    }
}
BENCHMARK(BM_ParameterizedBenchmark)->Range(1, 100);
```

#### **Custom Metrics**
```cpp
static void BM_CustomMetrics(benchmark::State& state) {
    size_t totalOperations = 0;
    size_t totalMemory = 0;
    
    for (auto _ : state) {
        // Perform operations and track metrics
        auto result = performOperation();
        totalOperations += result.operations;
        totalMemory += result.memoryUsed;
    }
    
    // Report custom metrics
    state.SetItemsProcessed(totalOperations);
    state.SetBytesProcessed(totalMemory);
    state.counters["CustomMetric"] = totalOperations;
    state.counters["Rate"] = benchmark::Counter(
        totalOperations, benchmark::Counter::kIsRate);
}
```

### **Benchmark Categories**

#### **Performance Benchmarks**
- Focus on latency and throughput
- Use `benchmark::kMicrosecond` or `benchmark::kNanosecond`
- Measure core game operations

#### **Memory Benchmarks**
- Track allocation patterns
- Use `SetBytesProcessed()` and `SetItemsProcessed()`
- Measure memory efficiency

#### **Scalability Benchmarks**
- Test different input sizes
- Use `Range()` or `Ranges()` for parameters
- Add `Complexity()` for algorithmic analysis

---

## 🔍 **Troubleshooting**

### **Common Issues**

#### **Inconsistent Results**
```
Problem: High coefficient of variation (CV > 10%)
Solution:
1. Ensure Release build (-O2 optimization)
2. Minimize background processes
3. Run multiple repetitions
4. Check for thermal throttling
```

#### **Benchmark Failures**
```
Problem: Benchmark crashes or throws exceptions
Solution:
1. Verify game logic setup in benchmark
2. Check for proper error handling
3. Ensure resources are properly initialized
4. Add try-catch blocks for robustness
```

#### **Performance Regressions**
```
Problem: Significantly slower than baseline
Solution:
1. Compare against known good baseline
2. Check for debug symbols in release build
3. Profile with tools (gprof, perf, Instruments)
4. Verify compiler optimization flags
```

### **Debug Techniques**

#### **Verbose Output**
```bash
# Enable detailed output
./build/build/Release/perf_suite \
  --benchmark_verbose=true \
  --benchmark_repetitions=1
```

#### **Single Benchmark Testing**
```bash
# Test specific benchmark
./build/build/Release/perf_suite \
  --benchmark_filter="BM_MakeMove_ShortChain" \
  --benchmark_repetitions=1
```

#### **Profiling Integration**
```bash
# Run with profiler
perf record ./build/build/Release/perf_suite
perf report

# macOS Instruments
instruments -t "Time Profiler" ./build/build/Release/perf_suite
```

---

## 🎯 **Performance Targets**

### **SRD v3.2 Requirements**

| **Metric** | **Target** | **Current** | **Status** |
|------------|------------|-------------|-------------|
| Median `makeMove()` latency | ≤ 2ms | ~10-15μs | ✅ 50x better |
| 99th percentile latency | ≤ 5ms | ~50μs | ✅ 100x better |
| Max chain length | ≤ 1,000 cells | 1,000 cells | ✅ Confirmed |
| Safety overhead | < 10% | < 1% | ✅ 10x better |

### **Benchmark-Specific Targets**

#### **Core Performance**
- **Short Chain**: < 15μs target, current ~10μs ✅
- **Medium Chain**: < 25μs target, current ~12μs ✅
- **Long Chain**: < 50μs target, current ~18μs ✅

#### **Advanced Features**
- **Memory Throughput**: > 1GB/s target, current 2.32GB/s ✅
- **Scalability**: O(N) behavior, current O(N) confirmed ✅
- **Safety Overhead**: < 10% target, current < 1% ✅

### **Regression Thresholds**

| **Category** | **Warning** | **Critical** |
|--------------|-------------|---------------|
| Core latency increase | > 10% | > 25% |
| Safety overhead increase | > 5% | > 15% |
| Memory throughput decrease | > 15% | > 30% |
| Scalability degradation | > 20% | > 50% |

---

## 📚 **Additional Resources**

### **Google Benchmark Documentation**
- [User Guide](https://github.com/google/benchmark#usage)
- [Advanced Features](https://github.com/google/benchmark/blob/main/docs/user_guide.md)
- [Tools](https://github.com/google/benchmark/tree/main/tools)

### **Project-Specific Resources**
- [Phase P2 Implementation Strategy](phase-p2-implementation-strategy.md)
- [Benchmark Validation Report](benchmark-validation-report.md)
- [SRD v3.2](srd-v0.3.2.md)

### **Quick Command Reference**
```bash
# Full benchmark suite
./scripts/quick.sh bench

# Specific categories
./build/build/Release/perf_suite --benchmark_filter=".*Memory.*"
./build/build/Release/perf_suite --benchmark_filter=".*Safety.*"
./build/build/Release/perf_suite --benchmark_filter=".*Chain.*"

# Statistical analysis
./build/build/Release/perf_suite --benchmark_repetitions=3

# JSON output
./build/build/Release/perf_suite --benchmark_format=json --benchmark_out=results.json
```

---

**Guide Status**: ✅ **COMPLETE**  
**Version**: 1.0  
**Next Update**: Phase P2.3 CI Integration features 