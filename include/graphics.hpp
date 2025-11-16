#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "SDL3/SDL.h"
#include "player.hpp"

class Graphics {
public:
        Graphics(const char* title, int width, int height);
        ~Graphics();

        Graphics& clear();
        Graphics& show();

        Graphics& drawMiniMap();
        Graphics& drawWorld();

private:
        int width;
        int height;
        SDL_Window* window;
        SDL_Renderer* renderer;
};

#endif
