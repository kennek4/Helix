#include "HLX_Pixel.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>

namespace HLX {
Pixel::Pixel(const SDL_FRect &data) { mData = data; };

const PixelState &Pixel::getState() { return mPixelState; };

const SDL_FRect &Pixel::getData() { return mData; };

const SDL_FColor &Pixel::getOutlineColor() { return mOutlineColor; };

const SDL_FColor &Pixel::getFillColor() { return mFillColor; };

void Pixel::setState(PixelState newState) { mPixelState = newState; };

void Pixel::setData(const SDL_FRect newData) { mData = newData; };

void Pixel::setOutlineColor(const SDL_FColor newColor) {
    mOutlineColor = newColor;
};

void Pixel::setFillColor(const SDL_FColor &newColor) { mFillColor = newColor; };

void Pixel::handleMouseClick(SDL_Event *event, const SDL_FColor &fillColor) {
    if (mPixelState != PixelState::FILLED)
        mPixelState = PixelState::FILLED;

    mFillColor = {fillColor.r, fillColor.g, fillColor.b, fillColor.a};
};

void Pixel::handleMouseHover(SDL_Event *event) {};

}; // namespace HLX
