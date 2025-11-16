#define SDL_MAIN_USE_CALLBACKS 1
#define WIDTH 1280
#define HEIGHT 720

#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
        const char* title = "Dungeon Crawler";
        if (!SDL_Init(SDL_INIT_VIDEO)) {
                SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }
        if (!SDL_CreateWindowAndRenderer(title, WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
                SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }
        SDL_SetRenderLogicalPresentation(renderer, WIDTH, HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
        if (event->type == SDL_EVENT_QUIT) {
                return SDL_APP_SUCCESS;
        }
        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
}
