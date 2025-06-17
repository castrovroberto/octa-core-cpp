/**
 * @file Direction.h
 * @brief Direction system for octagonal grid navigation
 * 
 * This file defines the Direction enum representing the 8 directions in an
 * octagonal grid, along with utility functions for direction manipulation.
 * 
 * The octagonal grid supports 8 directions at 45-degree intervals:
 * N, NE, E, SE, S, SW, W, NW
 * 
 * Rotation functions allow clockwise and counter-clockwise direction changes,
 * which are essential for the chain reaction algorithm in the game logic.
 */

#ifndef OCTA_CORE_DIRECTION_H
#define OCTA_CORE_DIRECTION_H

#include <cstdint>

/**
 * @enum Direction
 * @brief Represents the 8 directions in an octagonal grid
 * 
 * The directions are arranged in clockwise order starting from North.
 * Each direction corresponds to a 45-degree increment around the compass.
 * 
 * The numeric values are intentionally sequential to enable easy rotation
 * calculations using modular arithmetic.
 */
enum class Direction : uint8_t {
    N   = 0,  ///< North (0°)
    NE  = 1,  ///< North-East (45°)
    E   = 2,  ///< East (90°)
    SE  = 3,  ///< South-East (135°)
    S   = 4,  ///< South (180°)
    SW  = 5,  ///< South-West (225°)
    W   = 6,  ///< West (270°)
    NW  = 7   ///< North-West (315°)
};

/// Total number of directions in the octagonal grid
constexpr uint8_t NUM_DIRECTIONS = 8;

/**
 * @brief Rotates a direction clockwise by 45 degrees
 * @param d The current direction
 * @return The new direction after clockwise rotation
 * 
 * Examples:
 * - rotateClockwise(Direction::N) returns Direction::NE
 * - rotateClockwise(Direction::NW) returns Direction::N
 */
inline Direction rotateClockwise(Direction d) {
    return static_cast<Direction>((static_cast<uint8_t>(d) + 1) % NUM_DIRECTIONS);
}

/**
 * @brief Rotates a direction counter-clockwise by 45 degrees
 * @param d The current direction
 * @return The new direction after counter-clockwise rotation
 * 
 * Examples:
 * - rotateCounterClockwise(Direction::N) returns Direction::NW
 * - rotateCounterClockwise(Direction::NE) returns Direction::N
 */
inline Direction rotateCounterClockwise(Direction d) {
    return static_cast<Direction>((static_cast<uint8_t>(d) + NUM_DIRECTIONS - 1) % NUM_DIRECTIONS);
}

/**
 * @brief Rotates a direction by a specified number of 45-degree steps
 * @param d The current direction
 * @param steps Number of 45-degree steps (positive for clockwise, negative for counter-clockwise)
 * @return The new direction after rotation
 * 
 * This function is useful for more complex direction calculations.
 * 
 * Examples:
 * - rotateBySteps(Direction::N, 2) returns Direction::E (90° clockwise)
 * - rotateBySteps(Direction::E, -1) returns Direction::NE (45° counter-clockwise)
 */
inline Direction rotateBySteps(Direction d, int steps) {
    int newDirection = (static_cast<int>(d) + steps) % NUM_DIRECTIONS;
    if (newDirection < 0) {
        newDirection += NUM_DIRECTIONS;
    }
    return static_cast<Direction>(newDirection);
}

/**
 * @brief Gets the opposite direction (180-degree rotation)
 * @param d The current direction
 * @return The opposite direction
 * 
 * Examples:
 * - getOpposite(Direction::N) returns Direction::S
 * - getOpposite(Direction::NE) returns Direction::SW
 */
inline Direction getOpposite(Direction d) {
    return rotateBySteps(d, NUM_DIRECTIONS / 2);
}

/**
 * @brief Converts a Direction enum to its string representation
 * @param d The direction to convert
 * @return String representation of the direction
 * 
 * This function is useful for debugging and logging purposes.
 */
inline const char* directionToString(Direction d) {
    switch (d) {
        case Direction::N:  return "N";
        case Direction::NE: return "NE";
        case Direction::E:  return "E";
        case Direction::SE: return "SE";
        case Direction::S:  return "S";
        case Direction::SW: return "SW";
        case Direction::W:  return "W";
        case Direction::NW: return "NW";
        default:            return "INVALID";
    }
}

#endif // OCTA_CORE_DIRECTION_H 