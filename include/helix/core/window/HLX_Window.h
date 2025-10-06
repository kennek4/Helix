#pragma once

#include "../HLX_pch.h"

namespace HLX {

static bool isHelixWindowInitialized = false;

typedef struct SDLProps {
    SDL_Window *window;
    SDL_Renderer *renderer;
} SDLProps;

typedef struct WindowProps {
    int width;
    int height;
} WindowProps;

class Window {
  public:
    Window(SDLProps &sdlProps, WindowProps &windowProps);
    ~Window();

    bool init();
    bool shutdown();

  private:
    SDLProps *mSDLProps{nullptr};
    WindowProps *mWindowProps{nullptr};
};
}; // namespace HLX
