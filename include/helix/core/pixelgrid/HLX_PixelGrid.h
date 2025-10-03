#pragma once

#include "HLX_Pixel.h"

namespace HLX {

const static Uint32 SDL_EVENT_USER_ZOOM = SDL_RegisterEvents(1);

typedef float ZoomLevel;

typedef struct PixelGridState {
    int gridWidth = 0;
    int gridHeight = 0;

    SDL_Point minimumPoint{0, 0};
    SDL_Point middlePoint{0, 0};
    SDL_Point maximumPoint{0, 0};

    const ZoomLevel maxZoom{2.0f};
    const ZoomLevel minZoom{0.5f};
    ZoomLevel currentZoom{1.0f};

    PixelGridState() {};
    PixelGridState(const int gridWidth, const int gridHeight)
        : gridWidth(gridWidth), gridHeight(gridHeight) {};

} PixelGridState;

class PixelGrid {
  public:
    PixelGrid(PixelGridState *state, SDL_Renderer *renderer, int &windowWidth,
              int &windowHeight);
    ~PixelGrid();

    void create();
    void reset();

    void render();
    void handleMouseEvent(SDL_Event *event, SDL_FColor &color);
    void handleZoom(SDL_Event *event);
    void handleResize(SDL_Event *event);

  private:
    SDL_Renderer *mSDLRenderer{nullptr};
    PixelGridState *mState{nullptr};

    std::vector<HLX::Pixel *> mPixels{};
    Pixel *mHoveredPixel = nullptr;
};
}; // namespace HLX
