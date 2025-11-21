#pragma once

#include "Eigen/Dense"
#include "SDL3/SDL.h"
#include "entity.hpp"
#include "player.hpp"
#include <string>
#include <unordered_set>
#include <vector>

template <typename T> using Matrix4x4 = Eigen::Matrix<T, 4, 4>;
template <typename T> using Matrix3x4 = Eigen::Matrix<T, 3, 4>;
template <typename T> using Vector2 = Eigen::Vector<T, 2>;
template <typename T> using Vector4 = Eigen::Vector<T, 4>;
template <typename T> using RowVector4 = Eigen::RowVector<T, 4>;
template <typename T> using uset = std::unordered_set<T>;

class World {

public:
        World();
        World(const char* fname);
        ~World();

public:
        Vector2<int> findPlayerSpawn();
        World& drawPlayerPOV(SDL_Renderer* renderer, const Player* player);
        World& drawEntityInPlayerPOV(SDL_Renderer* renderer, const Player* player);
        operator std::string() const;

private:
        Matrix4x4<float> buildRotationMatrix(const Player* player);
        Matrix3x4<float> buildCameraMatrix(float fov);
        Matrix4x4<float> buildWall(Direction direction);
        Matrix4x4<float> buildFloor();
        Matrix4x4<float> buildCeil();

        uset<int> getVisableEntities(const Player* player);
        uset<int> filterVisableEntities(uset<int>* idx, EntityType etype);
        std::vector<Matrix4x4<float>> getVisableFloorVertices(const Player* player, uset<int>* idx);
        std::vector<Matrix4x4<float>> getVisableCeilingVertices(const Player* player, uset<int>* idx);
        std::vector<Matrix4x4<float>> getVisableWallVertices(const Player* player, uset<int>* idx);

        void getNextVisableEntity(const Player* player, uset<int>* idx, int i, int j);

        std::vector<Entity> level;
        int nRows;
        int nCols;
};
