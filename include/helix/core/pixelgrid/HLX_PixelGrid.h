#pragma once

#include "HLX_EventCallbackHandler.h"
#include "HLX_Palette.h"
#include "HLX_Pixel.h"
#include "HLX_Subscriber.h"
#include "HLX_Window.h"
#include <vector>

namespace HLX {

typedef float ZoomLevel;

typedef struct PixelGridState {
    int windowWidth = 0;
    int windowHeight = 0;
    int gridWidth = 0;
    int gridHeight = 0;

    SDL_Point minimumPoint{0, 0};
    SDL_Point middlePoint{0, 0};
    SDL_Point maximumPoint{0, 0};

    const ZoomLevel maxZoom{2.0f};
    const ZoomLevel minZoom{0.5f};
    ZoomLevel currentZoom{1.0f};

    SDL_Point mousePos{0, 0};
    float mouseScaleX = 1.0f;
    float mouseScaleY = 1.0f;

    SDL_Texture *background{nullptr};
    SDL_FRect backgroundRect = {0, 0, 0, 0};
    float backgroundTilingScale{1.0f};

    PixelGridState() {};
    PixelGridState(const int gridWidth, const int gridHeight)
        : gridWidth(gridWidth), gridHeight(gridHeight) {};

} PixelGridState;

class PixelGrid : public Subscriber {
  public:
    PixelGrid(PixelGridState *state, SDLProps &sdlProps, int &windowWidth,
              int &windowHeight, PaletteData *paletteData);
    ~PixelGrid();

    bool onNotify(SDL_Event *event) override;

    bool init();
    void reset();
    void render();

    void saveImage();

  private:
    SDLProps *mSDLProps{nullptr};
    PixelGridState *mState{nullptr};

    PaletteData *mPaletteData{nullptr};
    EventCallbackHandler mCallbackHandler{this};
    std::vector<HLX::Pixel *> mPixels{};

    void calculateBounds(int &newWidth, int &newHeight);
    void registerWindowCallbacks();
    void registerToolCallbacks();

    int getPixelIndex();
};
}; // namespace HLX
