#include "graphics.hpp"

Graphics::Graphics(const char* title, int width, int height) {
        if (!SDL_WasInit(SDL_INIT_VIDEO)) {
                SDL_Init(SDL_INIT_VIDEO);
        }
        this->width = width;
        this->height = height;
        this->window = SDL_CreateWindow(title, this->width, this->height, 0);
        this->renderer = SDL_CreateRenderer(this->window, NULL);
}

Graphics& Graphics::clear() {
        SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(this->renderer);
        return *this;
}

Graphics& Graphics::drawWorld() { return *this; }

Graphics& Graphics::drawMiniMap() {
        float mapScale = 0.3;
        float mapOffset = 0.01;

        int minDim = this->height;
        if (this->height > this->width) {
                minDim = this->width;
        }

        int mapX = int(mapOffset * minDim);
        int mapY = int(mapOffset * minDim);
        int mapW = int(mapScale * minDim);
        int mapH = int(mapScale * minDim);

        SDL_SetRenderDrawColor(this->renderer, 245, 245, 245, SDL_ALPHA_OPAQUE);
        SDL_RenderLine(this->renderer, mapX, mapY, mapX + mapW, mapY);
        SDL_RenderLine(this->renderer, mapX + mapW, mapY, mapX + mapW, mapY + mapH);
        SDL_RenderLine(this->renderer, mapX + mapW, mapY + mapH, mapX, mapY + mapH);
        SDL_RenderLine(this->renderer, mapX, mapY + mapH, mapX, mapY);

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
