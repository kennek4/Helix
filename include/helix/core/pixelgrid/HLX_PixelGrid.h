#pragma once

#include "../HLX_pch.h"
#include <HLX_Constants.h>
#include <HLX_EventCallbackHandler.h>
#include <HLX_EventSystem.h>
#include <HLX_Subscriber.h>
#include <HLX_Types.h>

namespace HLX {

class PixelGrid : public Subscriber {

  public:
    PixelGrid(SDLProps *sdlProps, const int GRID_WIDTH, const int GRID_HEIGHT);
    ~PixelGrid();

    bool onNotify(SDL_Event *event) override;

    bool init();
    void reset();

    inline const SDL_FRect &getBackgroundFRect() { return mBackgroundFRect; };
    inline const Grid &getGridData() { return mGrid; };
    inline bool *getIsActive() { return &mIsActive; };

  private:
    Grid mGrid;
    SDLProps *mSDLProps{nullptr};
    EventCallbackHandler mCallbackHandler{this};

    SDL_FPoint mMousePos;

    SDL_Point mMinPoint;
    SDL_Point mMidPoint;
    SDL_Point mMaxPoint;

    SDL_FRect mBackgroundFRect;

    bool mIsActive = true;

    void registerWindowCallbacks();
    void registerToolCallbacks();

    void handleBrushEvent(const SDL_Point &startPoint,
                          const SDL_FColor &brushColor, const int &brushSize,
                          const bool isPixelActive,
                          std::vector<int> &brushIndicies);

    void handleBucketEvent(const SDL_Point &startPoint,
                           const SDL_FColor &bucketColor);

    void handlePipetteEvent();

    void setGridBounds(const int newWindowWidth, const int newWindowHeight);

    bool isPointInGrid(const SDL_Point &point);
    bool isPixelEmpty(const int &pixelIndex);
};
}; // namespace HLX
