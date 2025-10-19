#pragma once

#include "../HLX_pch.h"
#include <HLX_Constants.h>
#include <HLX_Types.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_pixels.h>

namespace HLX {

class Toolbox {
  public:
    Toolbox() {
        SDL_zero(mEvent);
        mEvent.type = Constants::HelixEvent;
        mEvent.user.data1 = &mToolProps;
    };
    ~Toolbox() {};

    void setTool(Sint32 toolCode) { mEvent.user.code = toolCode; };
    void useTool(int mouseButton) {
        if (mouseButton == SDL_BUTTON_LEFT) {
            mToolProps.currentColor = &mToolProps.primary;
        } else {
            mToolProps.currentColor = &mToolProps.secondary;
        };

        SDL_PushEvent(&mEvent);
    };

    inline void setToolToPrimaryColor() {
        mToolProps.currentColor = &mToolProps.primary;
    };

    inline void setToolToSecondaryColor() {
        mToolProps.currentColor = &mToolProps.secondary;
    };

    SDL_FColor *getToolColor() { return mToolProps.currentColor; };
    SDL_FColor *getPrimaryColor() { return &mToolProps.primary; };
    SDL_FColor *getSecondaryColor() { return &mToolProps.secondary; };
    int *getToolSize() { return &mToolProps.size; };

  private:
    ToolProps mToolProps;
    SDL_Event mEvent;
};
}; // namespace HLX
