#define SDL_MAIN_USE_CALLBACKS 1
#define WIDTH 1280
#define HEIGHT 720

#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "game.hpp"

struct AppState {
        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;
        Game* game;
};

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {

        const char* title = "Dungeon Crawler";
        if (!SDL_SetAppMetadata(title, "0.0.1", "com.millhiser.crawler")) {
                return SDL_APP_FAILURE;
        }

        AppState* state = (AppState*)SDL_calloc(1, sizeof(AppState));
        if (!state) {
                SDL_Log("Couldn't create AppState: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }
        if (!SDL_Init(SDL_INIT_VIDEO)) {
                SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }
        if (!SDL_CreateWindowAndRenderer(title, WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE, &state->window,
                                         &state->renderer)) {
                SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }
        SDL_SetRenderLogicalPresentation(state->renderer, WIDTH, HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

        state->game = new Game;
        state->game->setResourcePath(argv[0]);
        state->game->loadWorld("world1.txt");
        state->game->loadPlayer(0);

        *appstate = state;
        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
        AppState* state = (AppState*)appstate;
        if (event->type == SDL_EVENT_QUIT) {
                return SDL_APP_SUCCESS;
        }
        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
        AppState* state = (AppState*)appstate;
        SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 0);
        SDL_RenderClear(state->renderer);
        state->game->RenderDrawWorld(state->renderer);
        SDL_RenderPresent(state->renderer);
        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
        AppState* state = (AppState*)appstate;
        SDL_DestroyRenderer(state->renderer);
        SDL_DestroyWindow(state->window);
        delete state->game;
        SDL_free(state);
}
