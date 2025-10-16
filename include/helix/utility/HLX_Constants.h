#pragma once

#include "../HLX_pch.h"

namespace HLX {
namespace Constants {

inline constexpr SDL_FColor EmptyPixelFColor{1.0f, 1.0f, 1.0f,
                                             SDL_ALPHA_TRANSPARENT_FLOAT};

// Helix Event Codes
inline const Uint32 HelixEvent = SDL_RegisterEvents(1);
inline constexpr Sint32 HelixEventToolEraser = 0;
inline constexpr Sint32 HelixEventToolBrush = 1;
inline constexpr Sint32 HelixEventToolBucket = 2;
inline constexpr Sint32 HelixEventToolColorPicker = 3;
inline constexpr Sint32 HelixEventToolShape = 4;

}; // namespace Constants
}; // namespace HLX
