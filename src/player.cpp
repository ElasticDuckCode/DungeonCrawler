#include "player.hpp"
#include <iostream>

Player::Player() {
        this->location[0] = 0;
        this->location[1] = 0;
        this->direction = Direction::NORTH;
        this->fov = 100;
        this->viewDistance = 5;
}

Player::~Player() {
}

Player::operator std::string() const {
        std::stringstream ss;
        ss << "Player(" << std::endl;
        ss << "\tlocation=[" << this->location[0] << "," << this->location[1] << "]" << std::endl;

        std::string name;
        switch (this->direction) {
        case Direction::NORTH:
                name = "North";
                break;
        case Direction::SOUTH:
                name = "South";
                break;
        case Direction::EAST:
                name = "East";
                break;
        case Direction::WEST:
                name = "West";
                break;
        }
        ss << "\tdirection=" << name << std::endl;

        ss << ")";
        return ss.str();
}
