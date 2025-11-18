#pragma once

#include <string>

enum class EntityType : int { EMPTY = 0, WALL = 1, PLAYER_SPAWN = 2 };

class Entity {
public:
        Entity(int code);

        operator std::string() const;
        bool operator==(const EntityType etype) const;

private:
        EntityType code;
};
