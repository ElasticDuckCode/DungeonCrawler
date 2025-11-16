#include "world.hpp"
#include "SDL3/SDL.h"
#include <fstream>
#include <iostream>
#include <sstream>

World::World() {
        this->level = std::vector<Entity>();
        this->nRows = 0;
        this->nCols = 0;
}

World::World(const char* fname) : World() {

        std::ifstream file(fname);
        if (!file.is_open()) {
                std::cerr << "Unable to open file: " << fname << std::endl;
                return;
        }

        std::string line;
        while (std::getline(file, line)) {
                if (line.empty()) {
                        continue;
                }
                this->nRows++;
                std::istringstream iss(line);
                std::string field;
                while (std::getline(iss, field, ',')) {
                        this->level.push_back(Entity(std::stoi(field)));
                }
                bool firstNonEmptyRow = (this->nCols <= 0) && (this->level.size() > 0);
                if (firstNonEmptyRow) {
                        this->nCols = this->level.size();
                }
        }
}

World::~World() {
}

Eigen::Vector2<int> World::findPlayerSpawn() {
        Eigen::Vector2<int> location = {0, 0};

        int i, j = 0;
        bool isFound = false;
        for (i = 0; i < this->nRows; i++) {
                for (j = 0; j < this->nCols; j++) {
                        if (this->level[i * this->nRows + j] == EntityType::PLAYER_SPAWN) {
                                location[0] = i;
                                location[1] = i;
                                isFound = true;
                        }
                        if (isFound) {
                                break;
                        }
                }
                if (isFound) {
                        break;
                }
        }

        return location;
}

World::operator std::string() const {
        std::stringstream ss;
        ss << "World(" << std::endl;
        ss << "\tnRows=" << this->nRows << std::endl;
        ss << "\tnCols=" << this->nCols << std::endl;
        ss << "\tLevel=" << std::endl;
        for (int i = 0; i < this->nRows; i++) {
                ss << "\t\t";
                for (int j = 0; j < this->nCols; j++) {
                        ss << std::string(this->level[i * nRows + j]) << " ";
                }
                ss << std::endl;
        }
        ss << ")";
        return ss.str();
}
