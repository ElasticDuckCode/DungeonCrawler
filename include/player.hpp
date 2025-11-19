#pragma once

#include <Eigen/Dense>

enum class Direction { NORTH, SOUTH, EAST, WEST };

constexpr Direction Directions[4] = {
    Direction::NORTH,
    Direction::SOUTH,
    Direction::EAST,
    Direction::WEST,
};

class Player {
public: // methods
        Player();
        ~Player();
        operator std::string() const;

public: // members
        Eigen::Vector2<int> location;
        Direction direction;
        float fov;
};
