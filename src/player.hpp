#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Eigen/Dense"
#include <string>

class Player {
public:
        Player();
        ~Player();

        std::string log();

private:
        Eigen::Vector3d direction;
        Eigen::Vector3d location;
};

#endif
