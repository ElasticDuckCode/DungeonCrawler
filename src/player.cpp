
#include "player.hpp"
#include <sstream>
#include <string>

#define NORTH {0, 0, 1}
#define SOUTH {0, 0, -1}
#define EAST {1, 0, 0}
#define WEST {-1, 0, 0}

Player::Player() {
        this->direction = NORTH;
        this->location = {0, 0, 0};
}

Player::~Player() {}
