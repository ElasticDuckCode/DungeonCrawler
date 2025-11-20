#pragma once

#include "Eigen/Dense"
#include "SDL3/SDL.h"
#include "entity.hpp"
#include "player.hpp"
#include <string>
#include <unordered_set>
#include <vector>

class World {
public:
        World();
        World(const char* fname);
        ~World();

public:
        Eigen::Vector2<int> findPlayerSpawn();
        World& drawPlayerPOV(SDL_Renderer* renderer, const Player* player);
        World& drawEntityInPlayerPOV(SDL_Renderer* renderer, const Player* player);
        operator std::string() const;

private:
        Eigen::Matrix<float, 4, 4> buildRotationMatrix(float angle);
        Eigen::Matrix<float, 3, 4> buildCameraMatrix(float fov);
        Eigen::Matrix<float, 4, 4> buildWall(Direction direction);
        void getVisableEntities(const Player* player, std::unordered_set<int>* idx);
        void getNextVisableEntity(const Player* player, std::unordered_set<int>* idx, int i, int j);

        std::vector<Entity> level;
        int nRows;
        int nCols;
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
