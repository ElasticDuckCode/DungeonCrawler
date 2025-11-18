#pragma once

#include <Eigen/Dense>

enum class Direction { NORTH, SOUTH, EAST, WEST };

class Player {
public:
        Player();
        ~Player();

        Eigen::Vector2<int> location;
        Direction direction;
        operator std::string() const;
};
