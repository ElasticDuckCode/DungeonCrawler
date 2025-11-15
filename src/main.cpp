#include "SDL3/SDL.h"
#include "graphics.hpp"
#include "player.hpp"
#include <print>
#include <string>

int main(int argc, char* argv[]) {
        Graphics graphics;
        SDL_Event event;
        bool runningGame = true;

        // For debugging
        Player player;
        std::println("{}", player.log());

        while (runningGame) {
                while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_EVENT_QUIT) {
                                runningGame = false;
                        }
                }

                graphics.clear();
                graphics.drawWalls();
                graphics.show();
        }

        return 0;
}
