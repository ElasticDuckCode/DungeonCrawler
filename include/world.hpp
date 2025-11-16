#ifndef WORLD_HPP
#define WORLD_HPP

#include "Eigen/Dense"
#include "entity.hpp"
#include <string>
#include <vector>

class World {
public:
        World();
        World(const char* fname);
        ~World();

        Eigen::Vector2<int> findPlayerSpawn();

        operator std::string() const;

private:
        std::vector<Entity> level;
        int nRows;
        int nCols;
};

#endif
