#pragma once

#include "imgui.h"
#include "pixelgrid/HLX_PixelGrid.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <vector>

namespace HLX {

typedef int BrushSize;
typedef std::vector<SDL_Point *> BrushOffsets;

typedef struct Brush {
    ImVec4 rawColor{0.2f, 0.2f, 0.2f, 1.0f};
    SDL_FColor fillColor{0.2f, 0.2f, 0.2f, SDL_ALPHA_OPAQUE};

    // In pixels, 1 would be a 1x1 brush, 3 would be a 3x3 brush, etc.
    BrushSize size{1};
    BrushOffsets offsets{};

    Brush() {
        // reserve enough offsets for 8x8 brush size
        offsets.reserve(64);
    };

    ~Brush() {
        for (SDL_Point *offset : offsets) {
            delete offset;
        };
    };
} Brush;

typedef struct HelixState {
    // SDL
    SDL_Window *window{nullptr};
    SDL_Renderer *renderer{nullptr};
    SDL_Texture *background{nullptr};
    SDL_FRect backgroundRect{0, 0, 0, 0};

    // Window Data
    float backgroundTilingScale{1.0f};
    int windowWidth{1280};
    int windowHeight{720};

    // ImGui

    // Brush Information
    Brush brush;

    // HLX
    PixelGrid *pixelGrid{nullptr};
    PixelGridState pixelGridState{};

} HelixState;

inline void changeBrushColor(ImVec4 &rawColor, SDL_FColor &brushColor) {
    brushColor = {rawColor.x, rawColor.y, rawColor.z, rawColor.w};
};

}; // namespace HLX
