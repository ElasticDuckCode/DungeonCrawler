#include "SDL3/SDL.h"
#include "graphics.hpp"

int main(int argc, char* argv[]) {
        Graphics graphics;
        SDL_Event event;
        bool runningGame = true;

        while (runningGame) {
                while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_EVENT_QUIT) {
                                runningGame = false;
                        }
                }

                graphics.clear();
                graphics.draw(0, 0);
                graphics.show();
        }

        return 0;
}
