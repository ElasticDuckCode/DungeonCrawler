#pragma once

#include <Eigen/Dense>

enum class Direction { NORTH = 0, SOUTH = 1, EAST = 2, WEST = 3 };

constexpr Direction Directions[4] = {
    Direction::NORTH,
    Direction::EAST,
    Direction::SOUTH,
    Direction::WEST,
};

constexpr Direction DirectionOrder[4][3] = {
    {Direction::WEST, Direction::EAST, Direction::SOUTH},
    {Direction::EAST, Direction::WEST, Direction::NORTH},
    {Direction::NORTH, Direction::SOUTH, Direction::WEST},
    {Direction::SOUTH, Direction::NORTH, Direction::EAST},
};

/* Eventually may want when more wise about drawing walls.
constexpr Direction DirectionOrder[4][4] = {
    {Direction::NORTH, Direction::WEST, Direction::EAST, Direction::SOUTH},
    {Direction::SOUTH, Direction::EAST, Direction::WEST, Direction::NORTH},
    {Direction::EAST, Direction::NORTH, Direction::SOUTH, Direction::WEST},
    {Direction::WEST, Direction::SOUTH, Direction::NORTH, Direction::EAST},
};
*/

class Player {
public: // methods
        Player();
        ~Player();
        operator std::string() const;

public: // members
        Eigen::Vector2<int> location;
        Direction direction;
        float fov;
        int viewDistance;
};
