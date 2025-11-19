#pragma once

#include <Eigen/Dense>

enum class Direction { NORTH, SOUTH, EAST, WEST };

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
