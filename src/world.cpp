#include "world.hpp"
#include "SDL3/SDL.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

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

World& World::drawPlayerPOV(SDL_Renderer* renderer, const Player* player) {

        float playerAngle = 0;

        switch (player->direction) {
        case Direction::NORTH:
                playerAngle = 0;
                break;
        case Direction::SOUTH:
                playerAngle = std::numbers::pi;
                break;
        case Direction::EAST:
                playerAngle = -std::numbers::pi / 2;
                break;
        case Direction::WEST:
                playerAngle = std::numbers::pi / 2;
                break;
        }

        // construct rotation matrix based on player direction
        Eigen::Matrix<float, 4, 4> R = this->buildRotationMatrix(playerAngle);

        // determine player location in world-space, centered on 1x1x1 grid tile
        Eigen::Vector4<float> playerPosition;
        playerPosition[0] = player->location[1] + 0.5;
        playerPosition[1] = 0.5;
        playerPosition[2] = this->nRows - (player->location[0] + 0.5);
        playerPosition[3] = 0;

        // Screen-space projection matrix
        Eigen::Matrix<float, 3, 4> P = this->buildCameraMatrix(player->fov);

        // get aspect ratio of screen
        int width, height;
        SDL_GetWindowSizeInPixels(SDL_GetRenderWindow(renderer), &width, &height);
        float aspect = float(width) / height;

        // check every grid in the level, and if non-empty entity, draw it.
        Eigen::Matrix<float, 4, 4> gridWalls = {};
        for (int j = 0; j < this->nCols; j++) {
                for (int i = 0; i < this->nRows; i++) {
                        if ((this->level[i * this->nCols + j] == EntityType::EMPTY) ||
                            (this->level[i * this->nCols + j] == EntityType::PLAYER_SPAWN)) {
                                continue;
                        }

                        for (Direction direction : Directions) {
                                if (direction != Direction::NORTH) {
                                        continue;
                                }

                                // TODO: Draw all walls needed rather than just one aligned with player
                                Eigen::Matrix<float, 4, 4> walls = this->buildWall(direction);

                                Eigen::Vector4<float> gridPosition;
                                gridPosition[0] = j + 0.5;
                                gridPosition[1] = 0.0;
                                gridPosition[2] = this->nRows - (i + 0.5);
                                gridPosition[3] = 0;

                                // Move wall to location of grid
                                gridWalls = walls.colwise() + gridPosition;

                                // Wall relatvive to player camera
                                gridWalls = gridWalls.colwise() - playerPosition;

                                // Rotate relative to player direction
                                gridWalls = R * gridWalls;

                                // Calculate average of vertices to determine if center is behind player.
                                Eigen::Vector4<float> b = {0.25, 0.25, 0.25, 0.25};
                                Eigen::RowVector4<float> a = {0, 0, 1, 0};
                                Eigen::Matrix<float, 1, 1> z = a * gridWalls * b;
                                std::cout << z[0, 0] << std::endl;
                                if (z[0, 0] <= 0) {
                                        continue;
                                }

                                // Project wall onto screen-space
                                Eigen::Matrix<float, 3, 4> gridScreen = P * gridWalls;
                                Eigen::RowVector<float, 4> gridScale = gridScreen.row(gridScreen.rows() - 1);
                                gridScreen = (gridScreen.array().rowwise() / gridScale.array()).matrix();

                                gridScreen.row(0) = gridScreen.row(0) * (width / aspect) / 2;
                                gridScreen.row(1) = gridScreen.row(1) * (1 * height) / 2;
                                gridScreen.row(0) = gridScreen.row(0).array() + width / 2;
                                gridScreen.row(1) = -gridScreen.row(1).array() + height / 2;

                                // Draw
                                std::vector<SDL_Vertex> verts{
                                    {
                                        SDL_FPoint{gridScreen[0, 0], gridScreen[1, 0]},
                                        SDL_FColor{0.5f, 0.5f, 0.5f, 1.0f},
                                        SDL_FPoint{0, 0},
                                    },
                                    {
                                        SDL_FPoint{gridScreen[0, 1], gridScreen[1, 1]},
                                        SDL_FColor{0.5f, 0.5f, 0.5f, 1.0f},
                                        SDL_FPoint{0, 0},
                                    },
                                    {
                                        SDL_FPoint{gridScreen[0, 2], gridScreen[1, 2]},
                                        SDL_FColor{0.5f, 0.5f, 0.5f, 1.0f},
                                        SDL_FPoint{0, 0},
                                    },
                                };
                                SDL_RenderGeometry(renderer, nullptr, verts.data(), verts.size(), nullptr, 0);

                                verts = {
                                    {
                                        SDL_FPoint{gridScreen[0, 2], gridScreen[1, 2]},
                                        SDL_FColor{0.5f, 0.5f, 0.5f, 1.0f},
                                        SDL_FPoint{0, 0},
                                    },
                                    {
                                        SDL_FPoint{gridScreen[0, 3], gridScreen[1, 3]},
                                        SDL_FColor{0.5f, 0.5f, 0.5f, 1.0f},
                                        SDL_FPoint{0, 0},
                                    },
                                    {
                                        SDL_FPoint{gridScreen[0, 0], gridScreen[1, 0]},
                                        SDL_FColor{0.5f, 0.5f, 0.5f, 1.0f},
                                        SDL_FPoint{0, 0},
                                    },
                                };
                                SDL_RenderGeometry(renderer, nullptr, verts.data(), verts.size(), nullptr, 0);

                                SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
                                SDL_RenderLine(renderer, gridScreen[0, 0], gridScreen[1, 0], gridScreen[0, 1],
                                               gridScreen[1, 1]);
                                SDL_RenderLine(renderer, gridScreen[0, 1], gridScreen[1, 1], gridScreen[0, 2],
                                               gridScreen[1, 2]);
                                SDL_RenderLine(renderer, gridScreen[0, 1], gridScreen[1, 1], gridScreen[0, 3],
                                               gridScreen[1, 3]);
                                SDL_RenderLine(renderer, gridScreen[0, 2], gridScreen[1, 2], gridScreen[0, 3],
                                               gridScreen[1, 3]);
                                SDL_RenderLine(renderer, gridScreen[0, 3], gridScreen[1, 3], gridScreen[0, 0],
                                               gridScreen[1, 0]);
                        }
                }
        }

        return *this;
}

World& World::drawEntityInPlayerPOV(SDL_Renderer* renderer, const Player* player) {
        return *this;
}

Eigen::Matrix<float, 4, 4> World::buildRotationMatrix(float angle) {
        // left-handed due to z-direction being forward in this game.
        // Therefore must negate angle.
        return Eigen::Matrix<float, 4, 4>{
            {std::cos(-angle), 0, std::sin(-angle), 0},
            {0, 1, 0, 0},
            {-std::sin(-angle), 0, std::cos(-angle), 0},
            {0, 0, 0, 1},
        };
}

Eigen::Matrix<float, 3, 4> World::buildCameraMatrix(float fov_d) {
        float fov = std::numbers::pi * fov_d / 180; // convert to radians
        float f = 1 / std::tan(fov / 2);
        return Eigen::Matrix<float, 3, 4>{
            {f, 0, 0, 0},
            {0, f, 0, 0},
            {0, 0, 1, 0},
        };
}

Eigen::Matrix<float, 4, 4> World::buildWall(Direction direction) {
        int idx = std::to_underlying(direction);

        Eigen::Matrix<float, 4, 4> walls;
        for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                        walls[i, j] = wallVerts[idx][i][j];
                }
        }
        walls.transposeInPlace();
        return walls;
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
