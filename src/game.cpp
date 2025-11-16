#include "game.hpp"

#include <print>

Game::Game() {
        this->world = World();
        this->players[0] = Player();
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
        this->players[number].location = this->world.findPlayerSpawn();
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
        std::stringstream playerss(this->players[0]);
        while (std::getline(playerss, line)) {
                ss << "\t" << line << std::endl;
        }
        ss << ")";
        return ss.str();
}
