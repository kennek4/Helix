#pragma once

#include "HLX_Pixel.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <vector>

namespace HLX {

const static Uint32 SDL_EVENT_USER_ZOOM = SDL_RegisterEvents(1);

typedef struct PixelGridProperties {
    int *windowWidth = nullptr;
    int *windowHeight = nullptr;

    int gridWidth = 0;
    int gridHeight = 0;
    float currentZoom = 1.0f;
    float minZoom = 0.5f;
    float maxZoom = 2.0f;
} PixelGridProperties;

class PixelGrid {
  public:
    PixelGrid(PixelGridProperties props, SDL_Renderer *renderer);
    ~PixelGrid();

    void create();
    void reset();

    void render();
    void handleMouseEvent(SDL_Event *event);
    void handleZoom(SDL_Event *event);
    void handleResize(SDL_Event *event);

  private:
    inline bool pointInRange(int value, int minValue, int maxValue) const;
    SDL_Point convertToGridCoords(int x, int y);

    PixelGridProperties mProps;

    SDL_Point mGridMinimumPoint{0, 0};
    SDL_Point mGridMiddlePoint{0, 0};
    SDL_Point mGridMaximumPoint{0, 0};

    std::vector<HLX::Pixel *> mPixels{};
    Pixel *mHoveredPixel = nullptr;

    SDL_Renderer *mSDLRenderer;
};
}; // namespace HLX
