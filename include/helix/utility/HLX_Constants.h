#pragma once

#include "../HLX_pch.h"
#include <HLX_Types.h>
#include <array>
#include <string_view>

namespace HLX {
namespace Constants {

// Helix Event Codes
inline const Uint32 HelixEvent = SDL_RegisterEvents(1);
inline constexpr Sint32 HelixEventToolBrush = 0;
inline constexpr Sint32 HelixEventToolEraser = 1;
inline constexpr Sint32 HelixEventToolBucket = 2;
inline constexpr Sint32 HelixEventToolColorPicker = 3;
inline constexpr Sint32 HelixEventToolShape = 4;
inline constexpr Sint32 EventGUIHasPriority = 5;
inline constexpr Sint32 EventGUINoPriority = 6;

inline constexpr std::array<Sint32, 5> ToolEventCodes{
    HelixEventToolBrush, HelixEventToolEraser, HelixEventToolBucket,
    HelixEventToolColorPicker, HelixEventToolShape};

inline constexpr bool PixelStateFilled{true};
inline constexpr bool PixelStateEmpty{false};

inline constexpr SDL_FColor EmptyPixelFColor{1.0f, 1.0f, 1.0f,
                                             SDL_ALPHA_TRANSPARENT_FLOAT};

inline constexpr SDL_FColor DefaultBrushFColor{0.0f, 0.0f, 0.0f,
                                               SDL_ALPHA_OPAQUE_FLOAT};

constexpr std::array<std::pair<Sint32, std::string_view>, 5>
    ToolEventCodeNamePairsArray{{
        {HelixEventToolBrush, std::string_view{"Brush"}},
        {HelixEventToolEraser, std::string_view{"Eraser"}},
        {HelixEventToolBucket, std::string_view{"Bucket"}},
        {HelixEventToolColorPicker, std::string_view{"Color Picker"}},
        {HelixEventToolShape, std::string_view{"Shape"}},
    }};

inline constexpr ConstMap<Sint32, std::string_view, 5> ToolEventCodeToNameMap{
    ToolEventCodeNamePairsArray};

constexpr std::array<std::string_view, 3> ColorSchemes{
    {std::string_view{"Dark"}, std::string_view{"Light"},
     std::string_view{"Classic"}}};

// HACK: Should be doing this programmatically but
// hard coding for testing
inline constexpr std::array<SDL_Point, 16> BrushSizePointOffsets = {
    SDL_Point{0, 0},     SDL_Point{-25, 0},   SDL_Point{-25, -25},
    SDL_Point{0, -25},   SDL_Point{25, -25},  SDL_Point{25, 0},
    SDL_Point{25, 25},   SDL_Point{0, 25},    SDL_Point{-25, 25},
    SDL_Point{-50, 25},  SDL_Point{-50, 0},   SDL_Point{-50, -25},
    SDL_Point{-50, -50}, SDL_Point{-25, -50}, SDL_Point{0, -50},
    SDL_Point{25, -50},
};

}; // namespace Constants
}; // namespace HLX
