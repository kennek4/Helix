#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

namespace HLX {

typedef struct SDLData {
    SDL_Window *window{nullptr};
    SDL_Renderer *renderer{nullptr};
} SDLData;

class Window {
  public:
    Window();
    ~Window();

    SDLData &getSDLData();

  private:
    SDLData mSDLData;
};
}; // namespace HLX
