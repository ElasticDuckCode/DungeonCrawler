#pragma once

#include <string>

enum class EntityType : int { EMPTY = 0, PLAYER = 1, FLOOR = 2, WALL = 4 };

class Entity {
public:
        Entity(int code);

        operator std::string() const;
        bool operator==(const EntityType etype) const;

private:
        EntityType code;
};

constexpr float wallVerts[4][4][4] = {
    // North Wall
    {
        {-0.5, 0.0, 0.5, 1.0}, // North-Bottom-Left
        {-0.5, 1.0, 0.5, 1.0}, // North-Top-Left
        {0.5, 1.0, 0.5, 1.0},  // North-Top-Right
        {0.5, 0.0, 0.5, 1.0},  // North-Bottom-Right
    },
    // South Wall
    {
        {-0.5, 0.0, -0.5, 1.0}, // South-Bottom-Left
        {-0.5, 1.0, -0.5, 1.0}, // South-Top-Left
        {0.5, 1.0, -0.5, 1.0},  // South-Top-Right
        {0.5, 0.0, -0.5, 1.0},  // South-Bottom-Right
    },
    // East Wall
    {
        {0.5, 0.0, 0.5, 1.0},  // North-Bottom-Right
        {0.5, 1.0, 0.5, 1.0},  // North-Top-Right
        {0.5, 1.0, -0.5, 1.0}, // South-Top-Right
        {0.5, 0.0, -0.5, 1.0}, // South-Bottom-Right
    },
    // West Wall
    {
        {-0.5, 0.0, 0.5, 1.0},  // North-Bottom-Left
        {-0.5, 1.0, 0.5, 1.0},  // North-Top-Left
        {-0.5, 1.0, -0.5, 1.0}, // South-Top-Left
        {-0.5, 0.0, -0.5, 1.0}, // South-Bottom-Left
    },
};

constexpr float floorVert[4][4] = {
    {-0.5, 0.0, 0.5, 1.0},  // North-Bottom-Left
    {0.5, 0.0, 0.5, 1.0},   // North-Bottom-Right
    {0.5, 0.0, -0.5, 1.0},  // South-Bottom-Right
    {-0.5, 0.0, -0.5, 1.0}, // South-Bottom-Left
};
