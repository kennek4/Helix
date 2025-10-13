#pragma once

#include <HLX_EventCallbackHandler.h>
#include <HLX_EventSystem.h>
#include <HLX_Gui.h>
#include <HLX_Palette.h>
#include <HLX_PixelGrid.h>
#include <HLX_Subscriber.h>
#include <HLX_Toolbox.h>
#include <HLX_Window.h>

namespace HLX {
typedef struct HelixState {
    // SDL
    SDLProps sdlProps;

    // Window Data
    WindowProps windowProps;
    float backgroundTilingScale{1.0f};
    int windowWidth{1280};
    int windowHeight{720};

    // ImGui

    // HLX
    Window *window{nullptr};
    PixelGrid *pixelGrid{nullptr};
    PixelGridState pixelGridState{};

    // Tools
    Palette palette{};
    Toolbox toolbox{};

} HelixState;

}; // namespace HLX
