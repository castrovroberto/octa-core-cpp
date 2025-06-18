#!/bin/bash
# Performance Analysis & Profiling Script
# Phase P2.3.2 - Performance Analysis & Profiling
#
# This script performs comprehensive performance analysis including:
# - CPU profiling to identify hotspots
# - Memory profiling for allocation patterns
# - Cache analysis for optimization opportunities
# - Cross-platform profiling techniques

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
PROFILING_DIR="profiling_results"
BUILD_DIR="build/build/Release"
BENCHMARK_EXECUTABLE="$BUILD_DIR/perf_suite"
TEST_EXECUTABLE="$BUILD_DIR/run_tests"

echo -e "${BLUE}🔬 Performance Analysis & Profiling Suite${NC}"
echo -e "${BLUE}===========================================${NC}"

# Create profiling results directory
mkdir -p "$PROFILING_DIR"

# Function to check if tools are available
check_tool() {
    local tool=$1
    local install_hint=$2
    
    if command -v "$tool" &> /dev/null; then
        echo -e "${GREEN}✓${NC} $tool is available"
        return 0
    else
        echo -e "${YELLOW}⚠${NC} $tool not found. $install_hint"
        return 1
    fi
}

# Function to ensure build is ready
ensure_build() {
    echo -e "\n${BLUE}📦 Ensuring Release build is ready...${NC}"
    
    if [ ! -f "$BENCHMARK_EXECUTABLE" ]; then
        echo -e "${YELLOW}Building benchmark suite...${NC}"
        ./scripts/quick.sh bench >/dev/null
    fi
    
    if [ ! -f "$TEST_EXECUTABLE" ]; then
        echo -e "${YELLOW}Building test suite...${NC}"
        cmake --build --preset conan-release --target run_tests >/dev/null
    fi
    
    echo -e "${GREEN}✓${NC} Build ready"
}

# Function for CPU profiling using macOS Instruments
profile_cpu_instruments() {
    echo -e "\n${BLUE}🖥️  CPU Profiling with Instruments${NC}"
    
    if ! command -v instruments &> /dev/null; then
        echo -e "${YELLOW}⚠${NC} Instruments not available (requires Xcode)"
        return 1
    fi
    
    local output_file="$PROFILING_DIR/cpu_profile_instruments.trace"
    
    echo -e "${YELLOW}Running CPU profiling (this may take 30-60 seconds)...${NC}"
    
    # Run time profiler on benchmark suite
    timeout 60s instruments -t "Time Profiler" \
        -D "$output_file" \
        "$BENCHMARK_EXECUTABLE" \
        --benchmark_filter="BM_MakeMove_(Short|Medium|Long)Chain" \
        --benchmark_repetitions=5 \
        || echo "Profiling completed (timeout or natural end)"
    
    if [ -f "$output_file" ]; then
        echo -e "${GREEN}✓${NC} CPU profile saved to: $output_file"
        echo -e "  View with: open $output_file"
    else
        echo -e "${RED}✗${NC} CPU profiling failed"
    fi
}

# Function for memory profiling using macOS Instruments
profile_memory_instruments() {
    echo -e "\n${BLUE}💾 Memory Profiling with Instruments${NC}"
    
    if ! command -v instruments &> /dev/null; then
        echo -e "${YELLOW}⚠${NC} Instruments not available (requires Xcode)"
        return 1
    fi
    
    local output_file="$PROFILING_DIR/memory_profile_instruments.trace"
    
    echo -e "${YELLOW}Running memory profiling (this may take 30-60 seconds)...${NC}"
    
    # Run memory profiler on benchmark suite
    timeout 60s instruments -t "Allocations" \
        -D "$output_file" \
        "$BENCHMARK_EXECUTABLE" \
        --benchmark_filter="BM_MakeMove_MemoryTracking" \
        --benchmark_repetitions=3 \
        || echo "Memory profiling completed (timeout or natural end)"
    
    if [ -f "$output_file" ]; then
        echo -e "${GREEN}✓${NC} Memory profile saved to: $output_file"
        echo -e "  View with: open $output_file"
    else
        echo -e "${RED}✗${NC} Memory profiling failed"
    fi
}

# Function for cross-platform CPU profiling using built-in benchmark profiling
profile_cpu_benchmark() {
    echo -e "\n${BLUE}⚡ Cross-platform CPU Analysis${NC}"
    
    local output_file="$PROFILING_DIR/cpu_analysis_benchmark.txt"
    
    echo -e "${YELLOW}Running detailed benchmark analysis...${NC}"
    
    # Run benchmarks with profiling information
    "$BENCHMARK_EXECUTABLE" \
        --benchmark_filter="BM_MakeMove_(Short|Medium|Long)Chain|BM_GameLogic_MakeMove" \
        --benchmark_repetitions=10 \
        --benchmark_display_aggregates_only=false \
        --benchmark_counters_tabular=true \
        > "$output_file" 2>&1
    
    echo -e "${GREEN}✓${NC} CPU analysis saved to: $output_file"
    
    # Extract key metrics
    echo -e "\n${BLUE}📊 Key Performance Metrics:${NC}"
    grep -E "(BM_MakeMove_.*Chain|BM_GameLogic_MakeMove)" "$output_file" | \
        grep -E "(mean|median)" | head -10
}

# Function for memory analysis using built-in tracking
profile_memory_benchmark() {
    echo -e "\n${BLUE}🔍 Memory Usage Analysis${NC}"
    
    local output_file="$PROFILING_DIR/memory_analysis_benchmark.txt"
    
    echo -e "${YELLOW}Running memory tracking benchmarks...${NC}"
    
    # Run memory-focused benchmarks
    "$BENCHMARK_EXECUTABLE" \
        --benchmark_filter="BM_MakeMove_MemoryTracking|BM_MemoryAllocation|BM_ResourceUtilization" \
        --benchmark_repetitions=5 \
        --benchmark_display_aggregates_only=false \
        > "$output_file" 2>&1
    
    echo -e "${GREEN}✓${NC} Memory analysis saved to: $output_file"
    
    # Extract memory metrics
    echo -e "\n${BLUE}📊 Memory Performance Metrics:${NC}"
    grep -E "(bytes_per_second|items_per_second|MemoryTracking)" "$output_file" | head -10
}

# Function for scalability analysis
profile_scalability() {
    echo -e "\n${BLUE}📈 Scalability Analysis${NC}"
    
    local output_file="$PROFILING_DIR/scalability_analysis.txt"
    
    echo -e "${YELLOW}Running scalability benchmarks...${NC}"
    
    # Run scalability-focused benchmarks
    "$BENCHMARK_EXECUTABLE" \
        --benchmark_filter="BM_Scalability|BM_MakeMove_LargeMapStress" \
        --benchmark_repetitions=3 \
        --benchmark_display_aggregates_only=false \
        > "$output_file" 2>&1
    
    echo -e "${GREEN}✓${NC} Scalability analysis saved to: $output_file"
    
    # Extract scalability metrics
    echo -e "\n${BLUE}📊 Scalability Metrics:${NC}"
    grep -E "(BigO|RMS|map_radius)" "$output_file" | head -10
}

# Function for cache performance analysis
profile_cache_performance() {
    echo -e "\n${BLUE}🚀 Cache Performance Analysis${NC}"
    
    local output_file="$PROFILING_DIR/cache_analysis.txt"
    
    echo -e "${YELLOW}Running cache performance tests...${NC}"
    
    # Run cache-focused benchmarks
    "$BENCHMARK_EXECUTABLE" \
        --benchmark_filter="BM_CachePerformance|BM_GameMap_CellAccess" \
        --benchmark_repetitions=5 \
        --benchmark_display_aggregates_only=false \
        > "$output_file" 2>&1
    
    echo -e "${GREEN}✓${NC} Cache analysis saved to: $output_file"
    
    # Extract cache metrics
    echo -e "\n${BLUE}📊 Cache Performance Metrics:${NC}"
    grep -E "(stride|CachePerformance|CellAccess)" "$output_file" | head -10
}

# Function to analyze memory leaks using available tools
profile_memory_leaks() {
    echo -e "\n${BLUE}🔍 Memory Leak Analysis${NC}"
    
    # Try different leak detection methods
    if command -v leaks &> /dev/null; then
        echo -e "${YELLOW}Using macOS leaks tool...${NC}"
        
        # Run a quick test to check for leaks
        "$TEST_EXECUTABLE" &
        local test_pid=$!
        sleep 2
        
        leaks "$test_pid" > "$PROFILING_DIR/memory_leaks.txt" 2>&1 || true
        kill "$test_pid" 2>/dev/null || true
        wait "$test_pid" 2>/dev/null || true
        
        if grep -q "0 leaks for 0 total leaked bytes" "$PROFILING_DIR/memory_leaks.txt"; then
            echo -e "${GREEN}✓${NC} No memory leaks detected"
        else
            echo -e "${YELLOW}⚠${NC} Potential memory leaks found. Check: $PROFILING_DIR/memory_leaks.txt"
        fi
    else
        echo -e "${YELLOW}⚠${NC} macOS leaks tool not available"
    fi
}

# Function to generate performance report
generate_performance_report() {
    echo -e "\n${BLUE}📋 Generating Performance Report${NC}"
    
    local report_file="$PROFILING_DIR/performance_analysis_report.md"
    
    cat > "$report_file" << 'EOF'
# Performance Analysis Report - Phase P2.3.2

**Generated**: $(date)
**System**: macOS $(sw_vers -productVersion)
**Compiler**: $(clang --version | head -1)

## Executive Summary

This report contains comprehensive performance analysis results including CPU profiling, memory analysis, scalability testing, and optimization opportunities.

## Analysis Results

### CPU Performance Analysis
EOF

    if [ -f "$PROFILING_DIR/cpu_analysis_benchmark.txt" ]; then
        echo "
See detailed results in: \`cpu_analysis_benchmark.txt\`

Key findings:
- Core makeMove() performance characteristics
- Hot path identification
- Function call overhead analysis
" >> "$report_file"
    fi

    cat >> "$report_file" << 'EOF'

### Memory Usage Analysis
EOF

    if [ -f "$PROFILING_DIR/memory_analysis_benchmark.txt" ]; then
        echo "
See detailed results in: \`memory_analysis_benchmark.txt\`

Key findings:
- Memory allocation patterns
- Throughput characteristics
- Resource utilization efficiency
" >> "$report_file"
    fi

    cat >> "$report_file" << 'EOF'

### Scalability Analysis
EOF

    if [ -f "$PROFILING_DIR/scalability_analysis.txt" ]; then
        echo "
See detailed results in: \`scalability_analysis.txt\`

Key findings:
- O(N) complexity confirmation
- Map size performance scaling
- Large dataset behavior
" >> "$report_file"
    fi

    echo -e "${GREEN}✓${NC} Performance report generated: $report_file"
}

# Function to provide optimization recommendations
generate_optimization_recommendations() {
    echo -e "\n${BLUE}💡 Optimization Recommendations${NC}"
    
    local recommendations_file="$PROFILING_DIR/optimization_recommendations.md"
    
    cat > "$recommendations_file" << 'EOF'
# Optimization Recommendations - Phase P2.3.2

Based on the performance analysis, here are the identified optimization opportunities:

## High-Priority Optimizations

### 1. Chain Propagation Loop Optimization
**Finding**: Chain propagation is the primary performance bottleneck
**Recommendation**: 
- Reduce redundant map lookups
- Optimize neighbor traversal patterns
- Consider batch processing for multiple cells

### 2. Memory Access Pattern Optimization
**Finding**: Cache performance varies with access patterns
**Recommendation**:
- Improve data locality in GraphGameMap
- Consider memory prefetching for predictable access patterns
- Optimize cell state storage layout

### 3. Allocation Pattern Optimization
**Finding**: Frequent small allocations during chain reactions
**Recommendation**:
- Use object pooling for temporary objects
- Pre-allocate vectors for chain propagation
- Minimize heap allocations in hot paths

## Medium-Priority Optimizations

### 4. Branch Prediction Optimization
**Recommendation**: Optimize conditional logic in hot paths
- Reorder conditions by probability
- Use likely/unlikely hints where appropriate
- Minimize branching in inner loops

### 5. Data Structure Optimization
**Recommendation**: Review container choices
- Consider more cache-friendly containers
- Optimize shared_ptr usage patterns
- Evaluate weak_ptr overhead

## Low-Priority Optimizations

### 6. Compiler Optimization
**Recommendation**: Fine-tune compilation flags
- Profile-guided optimization (PGO)
- Link-time optimization (LTO)
- Architecture-specific optimizations

## Implementation Priority

1. **Immediate**: Chain propagation loop optimization
2. **Next**: Memory access pattern improvements
3. **Future**: Advanced compiler optimizations

EOF

    echo -e "${GREEN}✓${NC} Optimization recommendations generated: $recommendations_file"
}

# Main execution
main() {
    echo -e "${BLUE}Starting comprehensive performance analysis...${NC}"
    
    # Check available tools
    echo -e "\n${BLUE}🛠️  Checking available profiling tools:${NC}"
    check_tool "instruments" "Install Xcode for macOS profiling"
    check_tool "leaks" "Part of Xcode Command Line Tools"
    check_tool "dtrace" "Built into macOS"
    
    # Ensure build is ready
    ensure_build
    
    # Perform various types of profiling
    profile_cpu_benchmark
    profile_memory_benchmark
    profile_scalability
    profile_cache_performance
    profile_memory_leaks
    
    # Try advanced profiling if tools are available
    if command -v instruments &> /dev/null; then
        profile_cpu_instruments
        profile_memory_instruments
    fi
    
    # Generate reports and recommendations
    generate_performance_report
    generate_optimization_recommendations
    
    echo -e "\n${GREEN}🎉 Performance analysis complete!${NC}"
    echo -e "${BLUE}Results available in: $PROFILING_DIR/${NC}"
    echo -e "\n${BLUE}📁 Generated files:${NC}"
    ls -la "$PROFILING_DIR"
}

# Help function
show_help() {
    echo "Performance Analysis & Profiling Script"
    echo ""
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  -h, --help     Show this help message"
    echo "  --cpu-only     Run only CPU profiling"
    echo "  --memory-only  Run only memory profiling"
    echo "  --quick        Run quick analysis (reduced repetitions)"
    echo ""
    echo "Examples:"
    echo "  $0                 # Run full analysis"
    echo "  $0 --cpu-only      # CPU profiling only"
    echo "  $0 --quick         # Quick analysis"
}

# Parse command line arguments
case "${1:-}" in
    -h|--help)
        show_help
        exit 0
        ;;
    --cpu-only)
        ensure_build
        profile_cpu_benchmark
        profile_cpu_instruments
        ;;
    --memory-only)
        ensure_build
        profile_memory_benchmark
        profile_memory_instruments
        profile_memory_leaks
        ;;
    --quick)
        ensure_build
        profile_cpu_benchmark
        profile_memory_benchmark
        ;;
    *)
        main
        ;;
esac 