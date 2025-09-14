#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>

int main() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Window *window =
        SDL_CreateWindow("Hello, World!", 1280, 720,
                         SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS);

    SDL_ShowWindow(window);

    bool finished = false;
    SDL_Event event;

    while (!finished) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                finished = true;
            };
        };
    };

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
};
