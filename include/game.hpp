#ifndef GAME_HPP
#define GAME_HPP

#include "SDL3/SDL.h"
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
        Game& RenderDrawWorld(SDL_Renderer* renderer);

        operator std::string() const;

private:
        World world;
        Player player;
        std::filesystem::path dataPath;
};

#endif
