#pragma once

#include "../HLX_pch.h"
#include <HLX_Constants.h>
#include <HLX_Types.h>

namespace HLX {

// HACK: Should be doing this programmatically but hard coding for testing
constexpr std::array<SDL_Point, 16> SIZE_OFFSETS = {
    SDL_Point{0, 0},     SDL_Point{-25, 0},   SDL_Point{-25, -25},
    SDL_Point{0, -25},   SDL_Point{25, -25},  SDL_Point{25, 0},
    SDL_Point{25, 25},   SDL_Point{0, 25},    SDL_Point{-25, 25},
    SDL_Point{-50, 25},  SDL_Point{-50, 0},   SDL_Point{-50, -25},
    SDL_Point{-50, -50}, SDL_Point{-25, -50}, SDL_Point{0, -50},
    SDL_Point{25, -50},
};

class Toolbox {
  public:
    Toolbox() {
        SDL_zero(mEvent);
        mEvent.type = Constants::HelixEvent;
        mEvent.user.data1 = &mToolProps;
    };
    ~Toolbox() {};

    void setTool(Sint32 toolCode) { mEvent.user.code = toolCode; };
    void useTool() { SDL_PushEvent(&mEvent); };

    SDL_FColor *getToolColor() { return &mToolProps.color; };
    int *getToolSize() { return &mToolProps.size; };

  private:
    ToolProps mToolProps;
    SDL_Event mEvent;
};
}; // namespace HLX
