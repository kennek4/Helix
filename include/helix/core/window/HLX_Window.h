#pragma once

#include "../HLX_pch.h"
#include <HLX_Types.h>

namespace HLX {

static bool isHelixWindowInitialized = false;

class Window {
  public:
    Window(SDLProps *sdlProps, WindowProps *windowProps);
    ~Window();

    bool init();
    bool shutdown();

  private:
    SDLProps *mSDLProps{nullptr};
    WindowProps *mWindowProps{nullptr};
};
}; // namespace HLX
