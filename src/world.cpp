#include "world.hpp"
#include "SDL3/SDL.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <print>
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
                        if (this->level[i * this->nCols + j] == EntityType::PLAYER) {
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

        // check every grid in the level, and if non-empty entity, mark it as visable.
        Eigen::Matrix<float, 4, 4> gridWalls = {};
        std::unordered_set<int> visableEntityIdx{};
        this->getVisableEntities(player, &visableEntityIdx);

        // For each entity, detrmine which walls need to be drawn.
        std::vector<Eigen::Matrix<float, 4, 4>> wallVector;
        Eigen::RowVector4<float> v1{0, 0, 1, 0};
        Eigen::Vector4<float> v2{0.25, 0.25, 0.25, 0.25};
        Eigen::RowVector4<float> v3{1, 0, 0, 0};

        std::println("There are {} visable entities.", visableEntityIdx.size());

        for (int idx : visableEntityIdx) {
                int i = idx / this->nCols;
                int j = idx % this->nCols;

                Eigen::Vector4<float> gridPosition;
                gridPosition[0] = j + 0.5;
                gridPosition[1] = 0.0;
                gridPosition[2] = this->nRows - (i + 0.5);
                gridPosition[3] = 0;

                // TODO: need to draw all directions
                int playerDirInt = std::to_underlying(player->direction);
                for (Direction direction : DirectionOrder[playerDirInt]) {
                        // TODO: Permit drawing wall if there isn't one in front.

                        Eigen::Matrix<float, 4, 4> walls = this->buildWall(direction);

                        // Move wall to location of grid
                        gridWalls = walls.colwise() + gridPosition;

                        // Wall relatvive to player camera
                        gridWalls = gridWalls.colwise() - playerPosition;

                        // Rotate relative to player direction
                        gridWalls = R * gridWalls;

                        // Calculate average of vertices to determine if center is behind player.
                        float z = v1 * gridWalls * v2;
                        if (z <= 0) {
                                continue;
                        }

                        // Determine if left/right wall should be drawn
                        bool isRightWall = (Directions[(playerDirInt + 1) % 4] == direction);
                        bool isLeftWall = (Directions[(playerDirInt - 1) % 4] == direction);
                        float x = v3 * gridWalls * v2;
                        if (x < 0 && isLeftWall) {
                                continue;
                        }
                        if (x > 0 && isRightWall) {
                                continue;
                        }

                        wallVector.push_back(gridWalls);
                }
        }

        std::println("There are {} visable walls.", wallVector.size());

        // Sort the walls in terms of distance along focal z-axis
        std::sort(wallVector.begin(), wallVector.end(), [v1, v2](auto a, auto b) {
                float za = v1 * a * v2;
                float zb = v1 * b * v2;
                return za > zb;
        });

        std::println("There are {} visable walls after sort.", wallVector.size());

        for (auto gridWalls : wallVector) {
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
                        SDL_FColor{0.75f, 0.75f, 0.75f, 1.0f},
                        SDL_FPoint{0, 0},
                    },
                    {
                        SDL_FPoint{gridScreen[0, 1], gridScreen[1, 1]},
                        SDL_FColor{0.75f, 0.75f, 0.75f, 1.0f},
                        SDL_FPoint{0, 0},
                    },
                    {
                        SDL_FPoint{gridScreen[0, 2], gridScreen[1, 2]},
                        SDL_FColor{0.75f, 0.75f, 0.75f, 1.0f},
                        SDL_FPoint{0, 0},
                    },
                };
                SDL_RenderGeometry(renderer, nullptr, verts.data(), verts.size(), nullptr, 0);

                verts = {
                    {
                        SDL_FPoint{gridScreen[0, 2], gridScreen[1, 2]},
                        SDL_FColor{0.75f, 0.75f, 0.75f, 1.0f},
                        SDL_FPoint{0, 0},
                    },
                    {
                        SDL_FPoint{gridScreen[0, 3], gridScreen[1, 3]},
                        SDL_FColor{0.75f, 0.75f, 0.75f, 1.0f},
                        SDL_FPoint{0, 0},
                    },
                    {
                        SDL_FPoint{gridScreen[0, 0], gridScreen[1, 0]},
                        SDL_FColor{0.75f, 0.75f, 0.75f, 1.0f},
                        SDL_FPoint{0, 0},
                    },
                };
                SDL_RenderGeometry(renderer, nullptr, verts.data(), verts.size(), nullptr, 0);

                SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
                SDL_RenderLine(renderer, gridScreen[0, 0], gridScreen[1, 0], gridScreen[0, 1], gridScreen[1, 1]);
                SDL_RenderLine(renderer, gridScreen[0, 1], gridScreen[1, 1], gridScreen[0, 2], gridScreen[1, 2]);
                SDL_RenderLine(renderer, gridScreen[0, 1], gridScreen[1, 1], gridScreen[0, 3], gridScreen[1, 3]);
                SDL_RenderLine(renderer, gridScreen[0, 2], gridScreen[1, 2], gridScreen[0, 3], gridScreen[1, 3]);
                SDL_RenderLine(renderer, gridScreen[0, 3], gridScreen[1, 3], gridScreen[0, 0], gridScreen[1, 0]);
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

void World::getVisableEntities(const Player* player, std::unordered_set<int>* idx) {

        // Start at player location
        int i = player->location[0];
        int j = player->location[1];

        // Recursively call function to fill entities vector
        this->getNextVisableEntity(player, idx, i, j);

        return;
}

void World::getNextVisableEntity(const Player* player, std::unordered_set<int>* idx, int i, int j) {

        // Check that index doesn't exceed player view distance (Euclidean)
        int x = player->location[0];
        int y = player->location[1];
        int a = (x - i);
        int b = (y - j);
        int c = player->viewDistance;
        if ((a * a + b * b) > (c * c)) {
                return;
        }

        // Check that index exists in our world. Otherwise return.
        int vectorIdx = i * this->nCols + j;
        if (i >= this->nRows || j >= this->nCols || vectorIdx >= this->level.size()) {
                return;
        }

        // Start search outward for furthest blocks.
        switch (player->direction) {
        case Direction::NORTH:
                this->getNextVisableEntity(player, idx, i - 1, j + 1);
                this->getNextVisableEntity(player, idx, i - 1, j + 0);
                this->getNextVisableEntity(player, idx, i - 1, j - 1);
                break;
        case Direction::SOUTH:
                this->getNextVisableEntity(player, idx, i + 1, j + 1);
                this->getNextVisableEntity(player, idx, i + 1, j + 0);
                this->getNextVisableEntity(player, idx, i + 1, j - 1);
                break;
        case Direction::EAST:
                this->getNextVisableEntity(player, idx, i + 1, j + 1);
                this->getNextVisableEntity(player, idx, i + 0, j + 1);
                this->getNextVisableEntity(player, idx, i - 1, j + 1);
                break;
        case Direction::WEST:
                this->getNextVisableEntity(player, idx, i + 1, j - 1);
                this->getNextVisableEntity(player, idx, i + 0, j - 1);
                this->getNextVisableEntity(player, idx, i - 1, j - 1);
                break;
        }

        // Add entity to list if not the player and not empty.
        bool isEmpty = (this->level[vectorIdx] == EntityType::EMPTY);
        bool isPlayer = (this->level[vectorIdx] == EntityType::PLAYER);
        if (!isPlayer && !isEmpty) {
                idx->insert(vectorIdx);
        }
        return;
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
