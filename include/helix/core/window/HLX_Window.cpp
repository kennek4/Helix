#include "HLX_Window.h"
#include <SDL3/SDL_render.h>

namespace HLX {
Window::Window(SDLProps *sdlData, WindowProps *windowProps) {
    mSDLProps = sdlData;
    mWindowProps = windowProps;
};

Window::~Window() {
    mWindowProps = nullptr;
    mSDLProps = nullptr;
};

bool Window::init() {
    constexpr SDL_InitFlags INIT_FLAGS = SDL_INIT_VIDEO | SDL_INIT_EVENTS;
    constexpr SDL_WindowFlags WIN_FLAGS =
        SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIGH_PIXEL_DENSITY |
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_CAPTURE |
        SDL_WINDOW_MOUSE_RELATIVE_MODE | SDL_WINDOW_INPUT_FOCUS |
        SDL_WINDOW_FULLSCREEN;

    if (!SDL_Init(INIT_FLAGS)) {
        SDL_Log("%s", SDL_GetError());
        return false;
    };

    // NOTE: Get primary display bounds
    SDL_DisplayID *primaryDisplayID = new SDL_DisplayID;
    *primaryDisplayID = SDL_GetPrimaryDisplay();
    if (primaryDisplayID == nullptr) {
        SDL_Log("%s", SDL_GetError());
        SDL_free(primaryDisplayID);
        return false;
    };

    const SDL_DisplayMode *displayMode =
        SDL_GetDesktopDisplayMode(*primaryDisplayID);
    if (displayMode == nullptr) {
        SDL_Log("%s", SDL_GetError());
        SDL_free(primaryDisplayID);
        return false;
    };

    SDL_Log("Creating new Window/Renderer...");
    if (!SDL_CreateWindowAndRenderer(
            "Helix Sprite Maker", displayMode->w, displayMode->h, WIN_FLAGS,
            &mSDLProps->window, &mSDLProps->renderer)) {
        SDL_Log("[FAIL] SDL failed to create Window/Renderer: %s",
                SDL_GetError());
        SDL_free(primaryDisplayID);
        return false;
    };

    // TODO: Control whether SDL is set to fullscreen by config variable
    SDL_SetWindowFullscreen(mSDLProps->window, true);

    SDL_GetWindowSize(mSDLProps->window, &mWindowProps->width,
                      &mWindowProps->height);

    SDL_free(primaryDisplayID);
    return true;
};

bool Window::shutdown() { return true; };

}; // namespace HLX
