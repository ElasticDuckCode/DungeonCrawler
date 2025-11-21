#include "game.hpp"

#include <iostream>
#include <print>
#include <string>

#include "SDL3_image/SDL_image.h"

#define N_TEXTURES 10

Game::Game() {
        this->world = World();
        this->player = Player();
        this->textures = new SDL_Texture*[N_TEXTURES];
}

Game::~Game() {
        for (int i = 0; i < N_TEXTURES; i++) {
                SDL_DestroyTexture(this->textures[i]);
        }
        delete[] this->textures;
}

Game& Game::setResourcePath(const char* execPath) {
        if (this->dataPath.empty()) {
                this->dataPath = std::filesystem::relative(execPath, "./");
                this->dataPath = std::filesystem::absolute(this->dataPath);
                this->dataPath = this->dataPath.remove_filename();
                this->dataPath = this->dataPath.parent_path();
                this->dataPath = this->dataPath.parent_path();
                this->dataPath = this->dataPath.parent_path();
                this->dataPath = this->dataPath / "src/resources";
        }
        return *this;
}

Game& Game::loadWorld(const char* rfname) {
        std::filesystem::path fname = this->dataPath / rfname;
        this->world = World(fname.c_str());
        return *this;
}

Game& Game::loadPlayer(int number) {
        // TODO: player number does nothing right now.
        this->player.location = this->world.findPlayerSpawn();
        return *this;
}

Game& Game::loadTextures(SDL_Renderer* renderer, const char* rfname) {
        std::filesystem::path fname = this->dataPath / rfname;

        this->textures[0] = IMG_LoadTexture(renderer, (fname / "floor.png").c_str());
        this->textures[1] = IMG_LoadTexture(renderer, (fname / "ceil.png").c_str());
        this->textures[2] = IMG_LoadTexture(renderer, (fname / "wall.png").c_str());

        return *this;
}

Game& Game::RenderDrawWorld(SDL_Renderer* renderer) {
        // debugging info
        std::println("\033[2J\033[1;1H {}", std::string(*this));

        this->world.drawPlayerPOV(renderer, &this->player, this->textures);
        return *this;
}

Game::operator std::string() const {
        std::stringstream ss;
        ss << "Game(" << std::endl;
        std::stringstream worldss(this->world);
        std::string line;
        while (std::getline(worldss, line)) {
                ss << "\t" << line << std::endl;
        }
        std::stringstream playerss(this->player);
        while (std::getline(playerss, line)) {
                ss << "\t" << line << std::endl;
        }
        ss << ")";
        return ss.str();
}
