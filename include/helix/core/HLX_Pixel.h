#pragma once

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <map>

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

const std::map<PixelColors, SDL_Color> DEFAULT_COLORS{
    {PixelColors::RED, SDL_Color{255, 0, 0, SDL_ALPHA_OPAQUE}},
    {PixelColors::BLUE, SDL_Color{0, 0, 255, SDL_ALPHA_OPAQUE}},
    {PixelColors::YELLOW, SDL_Color{255, 255, 0, SDL_ALPHA_OPAQUE}},
    {PixelColors::GREEN, SDL_Color{0, 255, 0, SDL_ALPHA_OPAQUE}},
    {PixelColors::ORANGE, SDL_Color{255, 165, 0, SDL_ALPHA_OPAQUE}},
    {PixelColors::PURPLE, SDL_Color{128, 0, 128, SDL_ALPHA_OPAQUE}},
    {PixelColors::GREY, SDL_Color{128, 128, 128, SDL_ALPHA_OPAQUE}},
    {PixelColors::BLACK, SDL_Color{0, 0, 0, SDL_ALPHA_OPAQUE}},
    {PixelColors::WHITE, SDL_Color{255, 255, 255, SDL_ALPHA_OPAQUE}},
};

class Pixel {
  public:
    Pixel();
    Pixel(const SDL_FRect &data);
    ~Pixel() = default;

    // Getters / Setters
    const PixelState &getState();
    const SDL_FRect &getData();
    const SDL_Color &getOutlineColor();
    const SDL_Color &getFillColor();

    void setState(PixelState newState);
    void setData(const SDL_FRect newData);
    void setOutlineColor(const SDL_Color newColor);
    void setFillColor(const SDL_Color newColor);

    void handleMouseHover(SDL_Event *event);
    void handleMouseClick(SDL_Event *event, const SDL_Color &fillColor);

  private:
    PixelState mPixelState = PixelState::EMPTY;
    SDL_FRect mData{0};
    SDL_Color mOutlineColor{0, 0, 0, SDL_ALPHA_OPAQUE};
    SDL_Color mFillColor{255, 255, 255, SDL_ALPHA_OPAQUE};
};
}; // namespace HLX
