#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "SDL3/SDL.h"

class Graphics {
public:
        Graphics();
        Graphics(const char* title);
        ~Graphics();

        Graphics& clear();
        Graphics& show();

        Graphics& drawWalls();

private:
        SDL_Window* window;
        SDL_Renderer* renderer;
};

#endif
