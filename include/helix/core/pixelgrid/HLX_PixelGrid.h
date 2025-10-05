#pragma once

#include "HLX_EventCallbackHandler.h"
#include "HLX_Pixel.h"
#include "HLX_Subscriber.h"
#include <SDL3/SDL_surface.h>

namespace HLX {

// Custom user SDL_Event numbers
const static Uint32 EVENT_BRUSH_DOWN = SDL_RegisterEvents(1);
const static Uint32 EVENT_BRUSH_UP = SDL_RegisterEvents(1);

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

    SDL_Point mousePos{0, 0};

    PixelGridState() {};
    PixelGridState(const int gridWidth, const int gridHeight)
        : gridWidth(gridWidth), gridHeight(gridHeight) {};

} PixelGridState;

class PixelGrid : public Subscriber {
  public:
    PixelGrid(PixelGridState *state, SDL_Renderer *renderer, int &windowWidth,
              int &windowHeight);
    ~PixelGrid();

    bool onNotify(SDL_Event *event) override;

    void create();
    void reset();
    void render();

    void saveToSurface(SDL_Surface &surface);

  private:
    SDL_Renderer *mSDLRenderer{nullptr};
    PixelGridState *mState{nullptr};

    EventCallbackHandler mCallbackHandler{this};

    std::vector<HLX::Pixel *> mPixels{};
    Pixel *mHoveredPixel = nullptr;

    int getPixelIndex();
};
}; // namespace HLX
