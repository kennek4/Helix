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
    int canvasMaxX;
    int canvasMaxY;

    SDL_Point gridStartPoint{0, 0};
    int gridWidth = 32;
    int gridHeight = 32;
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
    PixelGridProperties mProps;
    std::vector<HLX::Pixel *> mPixels{};
    Pixel *mHoveredPixel = nullptr;

    SDL_Renderer *mSDLRenderer;
};
}; // namespace HLX
