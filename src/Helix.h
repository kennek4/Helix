#pragma once

#include "imgui.h"
#include "pixelgrid/HLX_PixelGrid.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

namespace HLX {
typedef struct HelixState {

    // SDL
    SDL_Window *window{nullptr};
    SDL_Renderer *renderer{nullptr};
    SDL_Texture *background{nullptr};
    float backgroundTilingScale{1.0f};

    // ImGui
    ImVec4 rawColor{0.2f, 0.2f, 0.2f, 1.0f};

    // Brush Information
    SDL_FColor brushColor{0.2f, 0.2f, 0.2f, SDL_ALPHA_OPAQUE};
    int brushSize{1};

    // HLX
    PixelGrid *pixelGrid{nullptr};
    float currentZoom{1.0f};
    const float minZoom{0.5f};
    const float maxZoom{2.0f};

    int windowWidth{1280};
    int windowHeight{720};

} HelixState;

inline void changeBrushColor(ImVec4 &rawColor, SDL_FColor &brushColor) {
    brushColor = {rawColor.x, rawColor.y, rawColor.z, rawColor.w};
};

}; // namespace HLX
