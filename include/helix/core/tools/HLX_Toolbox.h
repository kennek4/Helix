#pragma once

#include "../HLX_pch.h"
#include <array>
#include <map>
#include <vector>

namespace HLX {

const Uint32 HELIX_EVENT = SDL_RegisterEvents(1);
const Sint32 HELIX_EVENT_BRUSH = 1;
const Sint32 HELIX_EVENT_PENCIL = 2;
const Sint32 HELIX_EVENT_ERASER = 3;
const Sint32 HELIX_EVENT_BUCKET = 4;

// HACK: Should be doing this programmatically but hard coding for testing
constexpr std::array<SDL_Point, 16> SIZE_OFFSETS = {
    SDL_Point{0, 0},     SDL_Point{-25, 0},   SDL_Point{-25, -25},
    SDL_Point{0, -25},   SDL_Point{25, -25},  SDL_Point{25, 0},
    SDL_Point{25, 25},   SDL_Point{0, 25},    SDL_Point{-25, 25},
    SDL_Point{-50, 25},  SDL_Point{-50, 0},   SDL_Point{-50, -25},
    SDL_Point{-50, -50}, SDL_Point{-25, -50}, SDL_Point{0, -50},
    SDL_Point{25, -50},
};

typedef struct ToolProps {
    SDL_FColor color{0.0f, 0.0f, 0.0f, SDL_ALPHA_OPAQUE_FLOAT};
    int size{1};
} ToolProps;

class Toolbox {
  public:
    Toolbox() {
        SDL_Log("HELIX_EVENT ID: %ud", HELIX_EVENT);
        SDL_zero(mEvent);

        mEvent.type = HELIX_EVENT;
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
