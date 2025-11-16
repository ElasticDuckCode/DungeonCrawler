#include "SDL3/SDL.h"
#include "graphics.hpp"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

int main(int argc, char* argv[]) {
        Graphics graphics("Crawler", WINDOW_WIDTH, WINDOW_HEIGHT);
        SDL_Event event;
        bool runningGame = true;

        while (runningGame) {
                while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_EVENT_QUIT) {
                                runningGame = false;
                        }
                }
                graphics.clear();
                graphics.drawMiniMap();
                graphics.show();
        }

        return 0;
}
