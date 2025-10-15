#pragma once

#include "../HLX_pch.h"
#include "HLX_PixelGrid.h"

namespace HLX {

class Renderer {
  public:
    Renderer(SDL_Renderer *renderer);
    ~Renderer();

    void clearScreen();
    void createBackground(const SDL_FRect &backgroundFRect,
                          const float backgroundScale);
    void createGrid(const Grid &grid);
    void render();

  private:
    SDL_Renderer *mSDLRenderer{nullptr};
    SDL_Texture *mBackgroundTexture{nullptr};
};
}; // namespace HLX
