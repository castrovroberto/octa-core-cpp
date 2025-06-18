#ifndef OCTA_CORE_COORDINATE_H
#define OCTA_CORE_COORDINATE_H

#include <functional>

/**
 * @class Coordinate
 * @brief Represents a coordinate position on the game map.
 *
 * This class encapsulates x and y coordinates and provides
 * utility methods for coordinate operations.
 */
class Coordinate {
  public:
    /**
     * @brief Constructs a coordinate with given x and y values.
     * @param x The x-coordinate
     * @param y The y-coordinate
     */
    Coordinate(int x, int y) : x_(x), y_(y) {}

    /**
     * @brief Default constructor creates coordinate at origin.
     */
    Coordinate() : x_(0), y_(0) {}

    /**
     * @brief Gets the x-coordinate.
     * @return The x-coordinate value.
     */
    int getX() const { return x_; }

    /**
     * @brief Gets the y-coordinate.
     * @return The y-coordinate value.
     */
    int getY() const { return y_; }

    /**
     * @brief Sets the x-coordinate.
     * @param x The new x-coordinate value.
     */
    void setX(int x) { x_ = x; }

    /**
     * @brief Sets the y-coordinate.
     * @param y The new y-coordinate value.
     */
    void setY(int y) { y_ = y; }

    /**
     * @brief Equality comparison operator.
     * @param other The other coordinate to compare with.
     * @return True if coordinates are equal, false otherwise.
     */
    bool operator==(const Coordinate& other) const { return x_ == other.x_ && y_ == other.y_; }

    /**
     * @brief Inequality comparison operator.
     * @param other The other coordinate to compare with.
     * @return True if coordinates are not equal, false otherwise.
     */
    bool operator!=(const Coordinate& other) const { return !(*this == other); }

  private:
    int x_;
    int y_;
};

#endif  // OCTA_CORE_COORDINATE_H