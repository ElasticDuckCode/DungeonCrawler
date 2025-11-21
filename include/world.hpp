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
        std::unordered_set<int> filterVisableEntities(std::unordered_set<int>* idx, EntityType etype);

        std::vector<Entity> level;
        int nRows;
        int nCols;
};
