#pragma once

#include "../HLX_pch.h"

namespace HLX {

const Uint32 HELIX_EVENT = SDL_RegisterEvents(1);
const Sint32 HELIX_EVENT_BRUSH = 1;
const Sint32 HELIX_EVENT_PENCIL = 2;
const Sint32 HELIX_EVENT_ERASER = 3;
const Sint32 HELIX_EVENT_BUCKET = 4;

class Toolbar {
  public:
    Toolbar() {
        SDL_Log("HELIX_EVENT ID: %ud", HELIX_EVENT);
        SDL_zero(mEvent);

        mEvent.type = HELIX_EVENT;
    };
    ~Toolbar() {};

    void setTool(Sint32 toolCode) { mEvent.user.code = toolCode; };
    void useTool() { SDL_PushEvent(&mEvent); };

  private:
    SDL_Event mEvent;
};
}; // namespace HLX
