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

Vector2<int> World::findPlayerSpawn() {
        Vector2<int> location = {0, 0};

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

        // Screen-space projection matrix
        Matrix3x4<float> P = this->buildCameraMatrix(player->fov);

        // get aspect ratio of screen
        int width, height;
        SDL_GetWindowSizeInPixels(SDL_GetRenderWindow(renderer), &width, &height);
        float aspect = float(width) / height;

        // Obtain list of all entities which are visable.
        uset<int> visableEntityIdx = this->getVisableEntities(player);

        // For each entity, detrmine which walls need to be drawn.
        std::vector<Matrix4x4<float>> wallVector = this->getVisableWallVertices(player, &visableEntityIdx);

        // TODO: Floor entities always visable.
        std::vector<Matrix4x4<float>> floorVector = this->getVisableFloorVertices(player, &visableEntityIdx);

        // All visable entities have ceiling reguardless
        std::vector<Matrix4x4<float>> ceilVector = this->getVisableCeilingVertices(player, &visableEntityIdx);

        // Combine
        std::vector<Matrix4x4<float>> vertVector;
        vertVector.insert(vertVector.begin(), wallVector.begin(), wallVector.end());
        // vertVector.insert(vertVector.begin(), floorVector.begin(), floorVector.end());
        // vertVector.insert(vertVector.begin(), ceilVector.begin(), ceilVector.end());

        // Sort in terms of distance along focal z-axis
        RowVector4<float> v1{0, 0, 1, 0};
        Vector4<float> v2{0.25, 0.25, 0.25, 0.25};
        RowVector4<float> v3{1, 0, 0, 0};
        std::sort(vertVector.begin(), vertVector.end(), [v1, v2](auto a, auto b) {
                float za = v1 * a * v2;
                float zb = v1 * b * v2;
                return za > zb;
        });

        for (auto vertices : vertVector) {
                // Project wall onto screen-space
                Matrix3x4<float> gridScreen = P * vertices;
                RowVector4<float> gridScale = gridScreen.row(gridScreen.rows() - 1);
                gridScreen = (gridScreen.array().rowwise() / gridScale.array()).matrix();

                gridScreen.row(0) = gridScreen.row(0) * (width) / 2;
                gridScreen.row(1) = gridScreen.row(1) * (aspect * height) / 2;
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

Matrix4x4<float> World::buildRotationMatrix(const Player* player) {

        float angle = 0;

        switch (player->direction) {
        case Direction::NORTH:
                angle = 0;
                break;
        case Direction::SOUTH:
                angle = std::numbers::pi;
                break;
        case Direction::EAST:
                angle = -std::numbers::pi / 2;
                break;
        case Direction::WEST:
                angle = std::numbers::pi / 2;
                break;
        }

        // left-handed due to z-direction being forward in this game.
        // Therefore must negate angle.
        return Matrix4x4<float>{
            {std::cos(-angle), 0, std::sin(-angle), 0},
            {0, 1, 0, 0},
            {-std::sin(-angle), 0, std::cos(-angle), 0},
            {0, 0, 0, 1},
        };
}

Matrix3x4<float> World::buildCameraMatrix(float fov_d) {
        float fov = std::numbers::pi * fov_d / 180; // convert to radians
        float f = 1 / std::tan(fov / 2);
        return Matrix3x4<float>{
            {f, 0, 0, 0},
            {0, f, 0, 0},
            {0, 0, 1, 0},
        };
}

Matrix4x4<float> World::buildWall(Direction direction) {
        int idx = std::to_underlying(direction);

        Matrix4x4<float> walls;
        for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                        walls[i, j] = wallVerts[idx][i][j];
                }
        }
        walls.transposeInPlace();
        return walls;
}

Matrix4x4<float> World::buildFloor() {

        Matrix4x4<float> floor;
        for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                        floor[i, j] = floorVert[i][j];
                }
        }
        floor.transposeInPlace();
        return floor;
}

Matrix4x4<float> World::buildCeil() {

        Matrix4x4<float> ceil;
        for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                        ceil[i, j] = ceilVert[i][j];
                }
        }
        ceil.transposeInPlace();
        return ceil;
}

uset<int> World::getVisableEntities(const Player* player) {

        // Start at player location
        int i = player->location[0];
        int j = player->location[1];

        // Initilize unordered set to store results
        uset<int> idx;

        // Recursively call function to fill entities vector
        this->getNextVisableEntity(player, &idx, i, j);

        return idx;
}

void World::getNextVisableEntity(const Player* player, uset<int>* idx, int i, int j) {

        // Check that index doesn't exceed player view distance
        int x = player->location[0];
        int y = player->location[1];
        int a = (x - i);
        int b = (y - j);
        int c = player->viewDistance;
        // if ((a * a + b * b) > (c * c)) { //  (L2) circle
        // if ((std::abs(a) + std::abs(b)) > c) { // (L1) diamond
        if (std::max(a, b) > c) { // (LInf) square, looks best in our world since no fog
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
        // bool isEmpty = (this->level[vectorIdx] == EntityType::EMPTY);
        bool isPlayer = (this->level[vectorIdx] == EntityType::PLAYER);
        if (!isPlayer) {
                idx->insert(vectorIdx);
        }
        return;
}

uset<int> World::filterVisableEntities(uset<int>* idx, EntityType etype) {
        uset<int> idxOut{};
        for (int i : *idx) {
                if (this->level[i] == etype) {
                        idxOut.insert(i);
                }
        }
        return idxOut;
}

std::vector<Matrix4x4<float>> World::getVisableFloorVertices(const Player* player, uset<int>* idx) {

        // initalize return vector
        std::vector<Matrix4x4<float>> vertices;

        // determine which entities are walls
        uset<int> floorIdx = this->filterVisableEntities(idx, EntityType::FLOOR);

        // initalize matrices for wall loop
        Matrix4x4<float> gridFloor = {};
        RowVector4<float> v1{0, 0, 1, 0};
        Vector4<float> v2{0.25, 0.25, 0.25, 0.25};
        RowVector4<float> v3{1, 0, 0, 0};

        // determine player location in world-space, centered on 1x1x1 grid tile
        Vector4<float> playerPosition;
        playerPosition[0] = player->location[1] + 0.5;
        playerPosition[1] = 0.5;
        playerPosition[2] = this->nRows - (player->location[0] + 0.5);
        playerPosition[3] = 0;

        // construct rotation matrix based on player direction
        Matrix4x4<float> R = this->buildRotationMatrix(player);

        // build persistent floor vertices
        Matrix4x4<float> floor = this->buildFloor();

        for (int idx : floorIdx) {
                int i = idx / this->nCols;
                int j = idx % this->nCols;

                Eigen::Vector4<float> gridPosition;
                gridPosition[0] = j + 0.5;
                gridPosition[1] = 0.0;
                gridPosition[2] = this->nRows - (i + 0.5);
                gridPosition[3] = 0;

                // Move wall to location of grid
                gridFloor = floor.colwise() + gridPosition;

                // Wall relatvive to player camera
                gridFloor = gridFloor.colwise() - playerPosition;

                // Rotate relative to player direction
                gridFloor = R * gridFloor;

                // Calculate average of vertices to determine if center is behind player.
                float z = v1 * gridFloor * v2;
                if (z <= 0) {
                        continue;
                }

                vertices.push_back(gridFloor);
        }
        return vertices;
}

std::vector<Matrix4x4<float>> World::getVisableCeilingVertices(const Player* player, uset<int>* idx) {

        // initalize return vector
        std::vector<Matrix4x4<float>> vertices;

        // determine which entities are walls
        uset<int> Idx = *idx;

        // initalize matrices for wall loop
        Matrix4x4<float> gridCeil = {};
        RowVector4<float> v1{0, 0, 1, 0};
        Vector4<float> v2{0.25, 0.25, 0.25, 0.25};
        RowVector4<float> v3{1, 0, 0, 0};

        // determine player location in world-space, centered on 1x1x1 grid tile
        Vector4<float> playerPosition;
        playerPosition[0] = player->location[1] + 0.5;
        playerPosition[1] = 0.5;
        playerPosition[2] = this->nRows - (player->location[0] + 0.5);
        playerPosition[3] = 0;

        // construct rotation matrix based on player direction
        Matrix4x4<float> R = this->buildRotationMatrix(player);

        // build persistent floor vertices
        Matrix4x4<float> ceil = this->buildCeil();

        for (int idx : Idx) {
                int i = idx / this->nCols;
                int j = idx % this->nCols;

                Eigen::Vector4<float> gridPosition;
                gridPosition[0] = j + 0.5;
                gridPosition[1] = 0.0;
                gridPosition[2] = this->nRows - (i + 0.5);
                gridPosition[3] = 0;

                // Move wall to location of grid
                gridCeil = ceil.colwise() + gridPosition;

                // Wall relatvive to player camera
                gridCeil = gridCeil.colwise() - playerPosition;

                // Rotate relative to player direction
                gridCeil = R * gridCeil;

                // Calculate average of vertices to determine if center is behind player.
                float z = v1 * gridCeil * v2;
                if (z <= 0) {
                        continue;
                }

                vertices.push_back(gridCeil);
        }
        return vertices;
}

std::vector<Matrix4x4<float>> World::getVisableWallVertices(const Player* player, uset<int>* idx) {

        // initalize return vector
        std::vector<Matrix4x4<float>> vertices;

        // determine which entities are walls
        uset<int> wallIdx = this->filterVisableEntities(idx, EntityType::WALL);

        // initalize matrices for wall loop
        Matrix4x4<float> gridWalls = {};
        RowVector4<float> v1{0, 0, 1, 0};
        Vector4<float> v2{0.25, 0.25, 0.25, 0.25};
        RowVector4<float> v3{1, 0, 0, 0};

        // determine player location in world-space, centered on 1x1x1 grid tile
        Vector4<float> playerPosition;
        playerPosition[0] = player->location[1] + 0.5;
        playerPosition[1] = 0.5;
        playerPosition[2] = this->nRows - (player->location[0] + 0.5);
        playerPosition[3] = 0;

        // construct rotation matrix based on player direction
        Matrix4x4<float> R = this->buildRotationMatrix(player);

        for (int idx : wallIdx) {
                int i = idx / this->nCols;
                int j = idx % this->nCols;

                Eigen::Vector4<float> gridPosition;
                gridPosition[0] = j + 0.5;
                gridPosition[1] = 0.0;
                gridPosition[2] = this->nRows - (i + 0.5);
                gridPosition[3] = 0;

                int playerDirInt = std::to_underlying(player->direction);
                for (Direction direction : DirectionOrder[playerDirInt]) {
                        // TODO: Permit drawing wall if there isn't one in front.

                        Matrix4x4<float> walls = this->buildWall(direction);

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

                        vertices.push_back(gridWalls);
                }
        }
        return vertices;
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
