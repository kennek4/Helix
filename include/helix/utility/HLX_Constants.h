#pragma once

#include "../HLX_pch.h"

namespace HLX {
namespace Constants {

inline constexpr bool PixelStateFilled{true};
inline constexpr bool PixelStateEmpty{false};

inline constexpr SDL_FColor EmptyPixelFColor{1.0f, 1.0f, 1.0f,
                                             SDL_ALPHA_TRANSPARENT_FLOAT};

inline constexpr SDL_FColor DefaultBrushFColor{0.0f, 0.0f, 0.0f,
                                               SDL_ALPHA_OPAQUE_FLOAT};

// HACK: Should be doing this programmatically but hard coding for testing
inline constexpr std::array<SDL_Point, 16> BrushSizePointOffsets = {
    SDL_Point{0, 0},     SDL_Point{-25, 0},   SDL_Point{-25, -25},
    SDL_Point{0, -25},   SDL_Point{25, -25},  SDL_Point{25, 0},
    SDL_Point{25, 25},   SDL_Point{0, 25},    SDL_Point{-25, 25},
    SDL_Point{-50, 25},  SDL_Point{-50, 0},   SDL_Point{-50, -25},
    SDL_Point{-50, -50}, SDL_Point{-25, -50}, SDL_Point{0, -50},
    SDL_Point{25, -50},
};

// Helix Event Codes
inline const Uint32 HelixEvent = SDL_RegisterEvents(1);
inline constexpr Sint32 HelixEventToolEraser = 0;
inline constexpr Sint32 HelixEventToolBrush = 1;
inline constexpr Sint32 HelixEventToolBucket = 2;
inline constexpr Sint32 HelixEventToolColorPicker = 3;
inline constexpr Sint32 HelixEventToolShape = 4;

}; // namespace Constants
}; // namespace HLX
