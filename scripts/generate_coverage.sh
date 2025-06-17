#!/bin/bash

# P1.4 Test Coverage Report Generator
# This script builds the project with coverage enabled and generates HTML reports

set -e

echo "🧪 P1.4 Test Coverage Report Generator"
echo "======================================"

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "❌ Error: Please run this script from the project root directory"
    exit 1
fi

# Clean previous coverage data
echo "🧹 Cleaning previous coverage data..."
find . -name "*.gcda" -delete
find . -name "*.gcno" -delete
rm -rf coverage_report/

# Configure with coverage enabled
echo "⚙️ Configuring project with coverage enabled..."
cmake --preset conan-default -DENABLE_COVERAGE=ON -DBUILD_TESTS=ON

# Build the project
echo "🔨 Building project..."
cmake --build --preset conan-default

# Run tests to generate coverage data
echo "🧪 Running tests to generate coverage data..."
cd build/conan-default
./run_tests
cd ../..

# Check for gcov
if ! command -v gcov &> /dev/null; then
    echo "❌ Error: gcov is required but not installed"
    exit 1
fi

# Try to use gcovr if available, otherwise use lcov
if command -v gcovr &> /dev/null; then
    echo "📊 Generating coverage report with gcovr..."
    
    # Generate HTML report
    gcovr -r . \
          --exclude 'build/' \
          --exclude 'tests/' \
          --exclude 'bench/' \
          --html --html-details \
          -o coverage_report.html
    
    # Generate console summary
    echo ""
    echo "📈 Coverage Summary:"
    echo "==================="
    gcovr -r . \
          --exclude 'build/' \
          --exclude 'tests/' \
          --exclude 'bench/' \
          --print-summary
    
    echo ""
    echo "✅ Coverage report generated: coverage_report.html"
    
elif command -v lcov &> /dev/null; then
    echo "📊 Generating coverage report with lcov..."
    
    # Create coverage directory
    mkdir -p coverage_report
    
    # Capture coverage data
    lcov --capture --directory . --output-file coverage_report/coverage.info
    
    # Remove external files
    lcov --remove coverage_report/coverage.info '/usr/*' '*/tests/*' '*/bench/*' --output-file coverage_report/coverage_filtered.info
    
    # Generate HTML report
    genhtml coverage_report/coverage_filtered.info --output-directory coverage_report
    
    echo ""
    echo "✅ Coverage report generated in coverage_report/ directory"
    echo "   Open coverage_report/index.html in your browser"
    
else
    echo "⚠️ Warning: Neither gcovr nor lcov found. Generating basic gcov report..."
    
    # Create basic coverage report directory
    mkdir -p coverage_report
    
    # Find all .gcda files and generate reports
    find build/Debug -name "*.gcda" -exec gcov {} \;
    
    # Move .gcov files to coverage directory
    mv *.gcov coverage_report/ 2>/dev/null || true
    
    echo ""
    echo "✅ Basic gcov files generated in coverage_report/ directory"
fi

# Check if we meet the 70% threshold
if command -v gcovr &> /dev/null; then
    COVERAGE_PERCENT=$(gcovr -r . --exclude 'build/' --exclude 'tests/' --exclude 'bench/' --print-summary | grep "TOTAL" | awk '{print $4}' | sed 's/%//')
    
    if [ ! -z "$COVERAGE_PERCENT" ]; then
        echo ""
        echo "🎯 P1.4 Coverage Target Check:"
        echo "Required: ≥70%"
        echo "Achieved: $COVERAGE_PERCENT%"
        
        if (( $(echo "$COVERAGE_PERCENT >= 70" | bc -l) )); then
            echo "✅ P1.4 coverage requirement MET!"
        else
            echo "❌ P1.4 coverage requirement NOT met (need ≥70%)"
            exit 1
        fi
    fi
fi

echo ""
echo "🎉 Coverage report generation complete!" 