# Specify the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(OctaCore)

# Set the C++ standard to C++17 (or newer if you prefer)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the executable target and its source files
# We will add more .cpp files here as we create them
add_executable(
    octa-core
    src/Main.cpp
    src/core/GameCell.cpp
    src/map/GraphGameMap.cpp
    src/map/ArrayGameMap.cpp
    src/engine/GameEngine.cpp
)

# Specify the include directories where headers can be found
target_include_directories(
    octa-core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# --- For later: Adding testing with GoogleTest ---
# enable_testing()
#
# add_subdirectory(googletest) # Assuming googletest is in this directory
#
# add_executable(run_tests tests/GameMapTests.cpp)
#
# target_link_libraries(run_tests
#     PRIVATE
#         GTest::gtest_main
# )
