#include "world.hpp"
#include "SDL3/SDL.h"
#include <cmath>
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
                        if (this->level[i * this->nCols + j] == EntityType::PLAYER_SPAWN) {
                                location[0] = i;
                                location[1] = j;
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

World& World::drawPlayerPOV(SDL_Renderer* renderer, Player player) {

        // every grid tile can have 4 walls (8 coordinates relative to center of grid floor tile)
        Eigen::Matrix<float, 8, 4> wallVerts{
            {-0.5, 1.0, 0.5, 1},  // North-Top-Left
            {0.5, 1.0, 0.5, 1},   // North-Top-Right
            {-0.5, 0.0, 0.5, 1},  // North-Bottom-Left
            {0.5, 0.0, 0.5, 1},   // North-Bottom-Right
            {-0.5, 1.0, -0.5, 1}, // South-Top-Left
            {0.5, 1.0, -0.5, 1},  // South-Top-Right
            {-0.5, 0.0, -0.5, 1}, // South-Bottom-Left
            {0.5, 0.0, -0.5, 1},  // South-Bottom-Right
        };

        // create groups of indices clockwise for each wall starting bottom left
        // TODO: Pick right wall, south if looking north etc...
        float angle = 0;
        std::vector<int> wallIdx;
        switch (player.direction) {
        case Direction::NORTH:
                angle = 0;
                // wallIdx = {2, 0, 1, 3};
                wallIdx = {6, 4, 5, 7};
                break;
        case Direction::SOUTH:
                angle = std::numbers::pi;
                wallIdx = {6, 4, 5, 7};
                break;
        case Direction::EAST:
                angle = -std::numbers::pi / 2;
                wallIdx = {3, 1, 5, 7};
                break;
        case Direction::WEST:
                angle = std::numbers::pi / 2;
                wallIdx = {6, 4, 0, 2};
                break;
        }

        Eigen::Matrix<float, 4, 4> walls = {};
        for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                        walls[i, j] = wallVerts[wallIdx[i], j];
                }
        }
        walls.transposeInPlace(); // each column is now a vertex

        // construct rotation matrix based on player direction
        // left-handed due to z being forward, must negate angle.
        Eigen::Matrix<float, 4, 4> R{
            {std::cos(-angle), 0, std::sin(-angle), 0},
            {0, 1, 0, 0},
            {-std::sin(-angle), 0, std::cos(-angle), 0},
            {0, 0, 0, 1},
        };

        // determine player location in world-space, centered on 1x1x1 grid tile
        Eigen::Vector4<float> playerPosition;
        playerPosition[0] = player.location[1] + 0.5;
        playerPosition[1] = 0.5;
        playerPosition[2] = this->nRows - (player.location[0] + 0.5);
        playerPosition[3] = 0;

        // Screen-space projection matrix
        float fov = 100; // horizontal
        float f = 0;
        fov = std::numbers::pi * fov / 180;
        f = 1 / std::tan(fov / 2); // assuming that screen x-axis plus/minus 1.

        Eigen::Matrix<float, 3, 4> P{
            {f, 0, 0, 0},
            {0, f, 0, 0},
            {0, 0, 1, 0},
        };

        // get aspect ratio of screen
        int width, height;
        SDL_GetWindowSize(SDL_GetRenderWindow(renderer), &width, &height);
        float aspect = float(width) / height;

        // check every grid in the level, and if non-empty entity, draw it.
        Eigen::Matrix<float, 4, 4> gridWalls = {};
        for (int i = 0; i < this->nRows; i++) {
                for (int j = 0; j < this->nCols; j++) {
                        if ((this->level[i * this->nCols + j] == EntityType::EMPTY) ||
                            (this->level[i * this->nCols + j] == EntityType::PLAYER_SPAWN)) {
                                continue;
                        }

                        Eigen::Vector4<float> gridPosition;
                        gridPosition[0] = j + 0.5;
                        gridPosition[1] = 0.0;
                        gridPosition[2] = this->nRows - (i + 0.5);
                        gridPosition[3] = 0;

                        // Center relative to player.
                        std::cout << "Player: (" << player.location[0] << "," << player.location[1] << ")" << std::endl
                                  << std::endl;
                        std::cout << "Wall:   (" << i << "," << j << ")" << std::endl << std::endl;
                        std::cout << "Player Position:\n" << playerPosition.transpose() << std::endl << std::endl;
                        std::cout << "Grid Position:\n" << gridPosition.transpose() << std::endl << std::endl;

                        // Move wall to location of grid
                        gridWalls = walls.colwise() + gridPosition;

                        std::cout << "Wall on Grid:" << std::endl;
                        std::cout << gridWalls << std::endl << std::endl;

                        // Wall relatvive to player camera
                        gridWalls = gridWalls.colwise() - playerPosition;

                        std::cout << "Wall Relative to Player:" << std::endl;
                        std::cout << gridWalls << std::endl << std::endl;

                        // Rotate relative to player direction
                        gridWalls = R * gridWalls;

                        // Determine if wall behind player (and not visable);
                        // if (gridPosition[2] <= 0.0) {
                        //         continue;
                        // }

                        // Project wall onto screen-space
                        Eigen::Matrix<float, 3, 4> gridScreen = P * gridWalls;
                        Eigen::RowVector<float, 4> gridScale = gridScreen.row(gridScreen.rows() - 1);
                        gridScreen = (gridScreen.array().rowwise() / gridScale.array()).matrix();
                        std::cout << "Player Screen Wall:" << std::endl;
                        std::cout << gridScreen << std::endl << std::endl;

                        gridScreen.row(0) = gridScreen.row(0) * width / 2;
                        gridScreen.row(1) = gridScreen.row(1) * (aspect * height) / 2;
                        gridScreen.row(0) = gridScreen.row(0).array() + width / 2;
                        gridScreen.row(1) = -gridScreen.row(1).array() + height / 2;
                        std::cout << "Pixel Wall:" << std::endl;
                        std::cout << gridScreen << std::endl << std::endl;

                        // Draw
                        SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
                        SDL_RenderLine(renderer, gridScreen[0, 0], gridScreen[1, 0], gridScreen[0, 1],
                                       gridScreen[1, 1]);
                        SDL_RenderLine(renderer, gridScreen[0, 1], gridScreen[1, 1], gridScreen[0, 2],
                                       gridScreen[1, 2]);
                        SDL_RenderLine(renderer, gridScreen[0, 2], gridScreen[1, 2], gridScreen[0, 3],
                                       gridScreen[1, 3]);
                        SDL_RenderLine(renderer, gridScreen[0, 3], gridScreen[1, 3], gridScreen[0, 0],
                                       gridScreen[1, 0]);
                }
        }

        return *this;
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
