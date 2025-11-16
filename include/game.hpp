#ifndef GAME_HPP
#define GAME_HPP

#include "player.hpp"
#include "world.hpp"
#include <filesystem>
#include <string>

class Game {
public:
        Game();
        ~Game();

        Game& setResourcePath(const char* path);
        Game& loadWorld(const char* rfname);
        Game& loadPlayer(int number);

        operator std::string() const;

private:
        World world;
        Player players[1];
        std::filesystem::path dataPath;
};

#endif
