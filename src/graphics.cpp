#include "graphics.hpp"
#include "Eigen/Dense"
#include <iostream>

Graphics::Graphics() : Graphics("My Creative Title") {};

Graphics::Graphics(const char* title) {
        if (!SDL_WasInit(SDL_INIT_VIDEO)) {
                SDL_Init(SDL_INIT_VIDEO);
        }
        this->window = SDL_CreateWindow(title, 640, 480, 0);
        this->renderer = SDL_CreateRenderer(this->window, NULL);
}

Graphics& Graphics::clear() {
        SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(this->renderer);
        return *this;
}

Graphics& Graphics::drawWalls() {

        SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderLine(this->renderer, 10, 10, 100, 100);
        return *this;
}

Graphics& Graphics::show() {
        SDL_RenderPresent(this->renderer);
        return *this;
}

Graphics::~Graphics() {
        SDL_DestroyRenderer(this->renderer);
        SDL_DestroyWindow(this->window);
        SDL_Quit();
}
