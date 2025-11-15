#include "SDL3/SDL.h"

class Graphics {
public:
        Graphics();
        Graphics(const char* title);
        ~Graphics();

        Graphics& clear();
        Graphics& draw(int x, int y);
        Graphics& show();

private:
        SDL_Window* window;
        SDL_Renderer* renderer;
};
