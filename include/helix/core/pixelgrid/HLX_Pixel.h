#pragma once

#include "../HLX_pch.h"

namespace HLX {

typedef enum class PixelState { EMPTY, HOVER, FILLED } PixelState;

typedef enum class PixelColors {
    RED,
    BLUE,
    YELLOW,
    GREEN,
    ORANGE,
    PURPLE,
    GREY,
    BLACK,
    WHITE,
} PixelColors;

const std::map<PixelColors, SDL_FColor> DEFAULT_COLORS{
    {PixelColors::RED, SDL_FColor{255, 0, 0, SDL_ALPHA_OPAQUE}},
    {PixelColors::BLUE, SDL_FColor{0, 0, 255, SDL_ALPHA_OPAQUE}},
    {PixelColors::YELLOW, SDL_FColor{255, 255, 0, SDL_ALPHA_OPAQUE}},
    {PixelColors::GREEN, SDL_FColor{0, 255, 0, SDL_ALPHA_OPAQUE}},
    {PixelColors::ORANGE, SDL_FColor{255, 165, 0, SDL_ALPHA_OPAQUE}},
    {PixelColors::PURPLE, SDL_FColor{128, 0, 128, SDL_ALPHA_OPAQUE}},
    {PixelColors::GREY, SDL_FColor{128, 128, 128, SDL_ALPHA_OPAQUE}},
    {PixelColors::BLACK, SDL_FColor{0, 0, 0, SDL_ALPHA_OPAQUE}},
    {PixelColors::WHITE, SDL_FColor{255, 255, 255, SDL_ALPHA_OPAQUE}},
};

class Pixel {
  public:
    Pixel();
    Pixel(const SDL_FRect &data);
    ~Pixel() = default;

    // Getters / Setters
    const PixelState &getState();
    const SDL_FRect &getData();
    const SDL_FColor &getOutlineColor();
    const SDL_FColor &getFillColor();

    void setState(PixelState newState);
    void setData(const SDL_FRect newData);
    void setOutlineColor(const SDL_FColor newColor);
    void setFillColor(const SDL_FColor &newColor);

    void handleMouseHover(SDL_Event *event);
    void handleMouseClick(SDL_Event *event, SDL_FColor *fillColor);

  private:
    PixelState mPixelState = PixelState::EMPTY;
    SDL_FRect mData{0};
    SDL_FColor mOutlineColor{0.2f, 0.2f, 0.2f, SDL_ALPHA_OPAQUE};
    SDL_FColor mFillColor{1.0f, 1.0f, 1.0f, SDL_ALPHA_OPAQUE};
};
}; // namespace HLX
