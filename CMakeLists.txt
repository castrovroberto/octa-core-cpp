# Specify the minimum required version of CMake
cmake_minimum_required(VERSION 3.20)

# Set the project name and version
project(OctaCore VERSION 1.0.0 LANGUAGES CXX)

# Set C++20 standard as required by SRD v3.2
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add CMake modules path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include Conan integration if available
if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conan_toolchain.cmake")
    include("${CMAKE_CURRENT_BINARY_DIR}/conan_toolchain.cmake")
endif()

# Find dependencies
find_package(spdlog QUIET)

# Define the main executable target
add_executable(
    octa-core
    src/Main.cpp
    src/core/GameCell.cpp
    src/core/CellChange.cpp
    src/map/GraphGameMap.cpp
    src/map/ArrayGameMap.cpp
    src/engine/GameEngine.cpp
    src/logic/OctaGameLogic.cpp
)

# Define shared library for Godot integration
option(BUILD_GODOT_BINDING "Build shared library for Godot" OFF)
if(BUILD_GODOT_BINDING)
    add_library(
        octacore-godot SHARED
        src/core/GameCell.cpp
        src/core/CellChange.cpp
        src/map/GraphGameMap.cpp
        src/map/ArrayGameMap.cpp
        src/engine/GameEngine.cpp
        src/logic/OctaGameLogic.cpp
        src/bindings/c_api.cpp
    )
    
    target_include_directories(
        octacore-godot
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    
    # Set shared library properties for cross-platform compatibility
    if(WIN32)
        set_target_properties(octacore-godot PROPERTIES
            OUTPUT_NAME "octacore"
            SUFFIX ".dll"
        )
    elseif(APPLE)
        set_target_properties(octacore-godot PROPERTIES
            OUTPUT_NAME "octacore"
            SUFFIX ".dylib"
        )
    else()
        set_target_properties(octacore-godot PROPERTIES
            OUTPUT_NAME "octacore"
            SUFFIX ".so"
        )
    endif()
    
    # Link spdlog if available
    if(spdlog_FOUND)
        target_link_libraries(octacore-godot PRIVATE spdlog::spdlog)
        target_compile_definitions(octacore-godot PRIVATE OCTA_ENABLE_LOGGING)
    endif()
    
    # Compiler-specific options
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(octacore-godot PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

# Set target properties
target_include_directories(
    octa-core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link spdlog if available
if(spdlog_FOUND)
    target_link_libraries(octa-core PRIVATE spdlog::spdlog)
    target_compile_definitions(octa-core PRIVATE OCTA_ENABLE_LOGGING)
endif()

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(octa-core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Optional thread safety
option(OCTA_THREAD_SAFE "Enable thread-safe game map adapter" OFF)
if(OCTA_THREAD_SAFE)
    target_compile_definitions(octa-core PRIVATE OCTA_THREAD_SAFE)
    find_package(Threads REQUIRED)
    target_link_libraries(octa-core PRIVATE Threads::Threads)
endif()

# Test coverage configuration
option(ENABLE_COVERAGE "Enable test coverage reporting" OFF)
if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(octa-core PRIVATE --coverage -O0)
        target_link_libraries(octa-core PRIVATE --coverage)
        
        # Add coverage flags to tests as well
        if(BUILD_TESTS AND TARGET run_tests)
            target_compile_options(run_tests PRIVATE --coverage -O0)
            target_link_libraries(run_tests PRIVATE --coverage)
        endif()
        
        message(STATUS "Test coverage enabled")
    else()
        message(WARNING "Test coverage is only supported with GCC or Clang")
    endif()
endif()

# Testing configuration
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    find_package(GTest QUIET)
    
    if(GTest_FOUND)
        add_executable(
            run_tests
            tests/test_main.cpp
            # Source files needed for testing
            src/core/GameCell.cpp
            src/core/CellChange.cpp
            src/map/GraphGameMap.cpp
            src/map/ArrayGameMap.cpp
            src/engine/GameEngine.cpp
            src/logic/OctaGameLogic.cpp
        )
        
        target_link_libraries(
            run_tests
            PRIVATE
                GTest::gtest
                GTest::gtest_main
        )
        
        target_include_directories(
            run_tests
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/include
        )
        
        # Register tests with CTest
        include(GoogleTest)
        gtest_discover_tests(run_tests)
    else()
        message(WARNING "Google Test not found. Tests will not be built.")
    endif()
endif()

# Benchmarking configuration
option(BUILD_BENCHMARKS "Build performance benchmarks" OFF)
if(BUILD_BENCHMARKS)
    find_package(benchmark QUIET)
    if(benchmark_FOUND)
        add_executable(
            perf_suite
            bench/benchmark_main.cpp
            # Source files needed for benchmarking
            src/core/GameCell.cpp
            src/core/CellChange.cpp
            src/map/GraphGameMap.cpp
            src/map/ArrayGameMap.cpp
            src/engine/GameEngine.cpp
            src/logic/OctaGameLogic.cpp
        )
        
        target_link_libraries(
            perf_suite
            PRIVATE
                benchmark::benchmark
        )
        
        target_include_directories(
            perf_suite
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/include
        )
    else()
        message(WARNING "Google Benchmark not found. Benchmarks will not be built.")
    endif()
endif()
