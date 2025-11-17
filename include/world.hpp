#ifndef WORLD_HPP
#define WORLD_HPP

#include "Eigen/Dense"
#include "SDL3/SDL.h"
#include "entity.hpp"
#include "player.hpp"
#include <string>
#include <vector>

class World {
public:
        World();
        World(const char* fname);
        ~World();

        Eigen::Vector2<int> findPlayerSpawn();

        World& drawPlayerPOV(SDL_Renderer* renderer, Player player);

        operator std::string() const;

private:
        std::vector<Entity> level;
        int nRows;
        int nCols;
};

#endif
