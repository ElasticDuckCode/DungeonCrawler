#include "game.hpp"

#include <iostream>
#include <print>
#include <string>

Game::Game() {
        this->world = World();
        this->player = Player();
}

Game::~Game() {
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

Game& Game::RenderDrawWorld(SDL_Renderer* renderer) {
        // debugging info
        std::println("\033[2J\033[1;1H {}", std::string(*this));

        this->world.drawPlayerPOV(renderer, this->player);
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
