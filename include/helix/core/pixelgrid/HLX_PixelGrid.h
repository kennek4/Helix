#pragma once

#include "../HLX_pch.h"
#include "HLX_EventCallbackHandler.h"
#include "HLX_EventSystem.h"
#include "HLX_Subscriber.h"
#include "HLX_Toolbox.h" // TODO: Change this import to Helix Custom events thing?
#include "HLX_Window.h"
#include <SDL3/SDL_rect.h>

namespace HLX {

typedef struct Grid {
    std::vector<SDL_FRect> frects;
    std::vector<SDL_FColor> colors;
    std::vector<char> states;
    int widthInPixels{32};
    int heightInPixels{32};
} Grid;

class PixelGrid : public Subscriber {

  public:
    PixelGrid(SDLProps *sdlProps, const int GRID_WIDTH, const int GRID_HEIGHT);
    ~PixelGrid();

    bool onNotify(SDL_Event *event) override;

    bool init();
    void reset();

    inline const SDL_FRect &getBackgroundFRect() { return mBackgroundFRect; };
    inline const Grid &getGridData() { return mGrid; };

  private:
    Grid mGrid;
    SDLProps *mSDLProps{nullptr};
    EventCallbackHandler mCallbackHandler{this};

    SDL_FPoint mMousePos;

    SDL_Point mMinPoint;
    SDL_Point mMidPoint;
    SDL_Point mMaxPoint;

    SDL_FRect mBackgroundFRect;

    void handleBrushEvent(const SDL_Point &startPoint,
                          const SDL_FColor &brushColor, const int &brushSize,
                          const bool isPixelActive,
                          std::vector<int> &brushIndicies);

    void handleBucketEvent(const SDL_Point &startPoint,
                           const SDL_FColor &bucketColor);

    void handlePipetteEvent();

    void setGridBounds(const int newWindowWidth, const int newWindowHeight);
    void registerWindowCallbacks();
    void registerToolCallbacks();
};
}; // namespace HLX
