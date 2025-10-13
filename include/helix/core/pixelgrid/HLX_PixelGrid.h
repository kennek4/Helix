#pragma once

#include "HLX_EventCallbackHandler.h"
#include "HLX_EventSystem.h"
#include "HLX_Palette.h"
#include "HLX_Subscriber.h"
#include "HLX_Toolbox.h" // TODO: Change this import to Helix Custom events thing?
#include "HLX_Window.h"

namespace HLX {

typedef struct Pixels {
    std::vector<SDL_FRect> data;
    std::vector<SDL_FColor> color;
    std::vector<char> state;
} Pixels;

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
    PixelGrid(PixelGridState *state, SDLProps &sdlProps,
              PaletteData *paletteData);
    ~PixelGrid();

    bool onNotify(SDL_Event *event) override;

    bool init();

    void reset();
    void render();
    void saveImage();

    PixelGridState &getState() const { return *mState; };

  private:
    Pixels mPixels;

    SDLProps *mSDLProps{nullptr};
    PixelGridState *mState{nullptr};

    PaletteData *mPaletteData{nullptr};
    EventCallbackHandler mCallbackHandler{this};

    void calculateBounds(int &newWidth, int &newHeight);
    void registerWindowCallbacks();
    void registerToolCallbacks();

    int getPixelIndex();
};
}; // namespace HLX
