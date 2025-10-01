#include "HLX_PixelGrid.h"
#include "HLX_Pixel.h"
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <cmath>

bool isWithinGrid(SDL_Point *point, SDL_Point *minPoint, SDL_Point *maxPoint) {
    static bool inDomain{false};
    static bool inRange{false};

    inDomain = (point->x >= minPoint->x) && (point->x < maxPoint->x);
    inRange = (point->y >= minPoint->y) && (point->y < maxPoint->y);

    return inDomain && inRange;
};

namespace HLX {
PixelGrid::PixelGrid(PixelGridState *state, SDL_Renderer *renderer,
                     int &windowWidth, int &windowHeight)
    : mState(state), mSDLRenderer(renderer) {

    const int TOTAL_PIXEL_COUNT = mState->gridWidth * mState->gridHeight;
    mPixels.reserve(TOTAL_PIXEL_COUNT);

    // NOTE: Set alpha blending mode
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // NOTE: Calculate grid position
    mState->middlePoint = {windowWidth / 2, windowHeight / 2};
    SDL_Log("Mid Point: %d, %d", mState->middlePoint.x, mState->middlePoint.y);

    // NOTE: 25 would be the side length
    // NOTE: These point values are at the CENTRE of the Pixel,
    // this means that SDL_Rects are off by +25 on the X and Y axises
    mState->minimumPoint.x =
        mState->middlePoint.x - ((mState->gridWidth / 2) * 25);
    mState->minimumPoint.y =
        mState->middlePoint.y - ((mState->gridHeight / 2) * 25);
    SDL_Log("Min Point: %d, %d", mState->minimumPoint.x,
            mState->minimumPoint.y);

    mState->maximumPoint.x =
        mState->middlePoint.x + ((mState->gridWidth / 2) * 25);
    mState->maximumPoint.y =
        mState->middlePoint.y + ((mState->gridHeight / 2) * 25);
    SDL_Log("Max Point: %d, %d", mState->maximumPoint.x,
            mState->maximumPoint.y);

    const float sideLength = 25 * mState->currentZoom;

    // NOTE: Offset X, Y positions to account for SDL_Rect origin being top left
    // of the rectangle
    SDL_FRect *pixelData = new SDL_FRect;
    pixelData->x = mState->minimumPoint.x;
    pixelData->y = mState->minimumPoint.y;
    pixelData->w = pixelData->h = 25;

    mPixels.emplace_back(new Pixel(*pixelData));
    // NOTE: Add all points except first and last
    for (int y = 1; y < mPixels.capacity(); y++) {
        if (y % mState->gridHeight == 0) {
            pixelData->x = mState->minimumPoint.x;
            pixelData->y += sideLength;
        } else {
            pixelData->x += sideLength;
        }

        mPixels.emplace_back(new Pixel(*pixelData));
    };

    delete pixelData;
};

PixelGrid::~PixelGrid() {
    for (Pixel *pixel : mPixels) {
        delete pixel;
    };
};

void PixelGrid::reset() {
    if (mPixels.size() == 0)
        return;
    for (Pixel *pixel : mPixels) {
        pixel->setFillColor(DEFAULT_COLORS.at(PixelColors::WHITE));
    };
};

void PixelGrid::render() {
    static SDL_FColor pixelColor{255, 255, 255, SDL_ALPHA_OPAQUE};
    static PixelState pixelState{PixelState::EMPTY};

    // HACK: This shit looks wack, def better ways
    for (Pixel *pixel : mPixels) {
        pixelState = pixel->getState();
        if (pixelState == PixelState::FILLED) {
            pixelColor = pixel->getFillColor();
            SDL_SetRenderDrawColorFloat(mSDLRenderer, pixelColor.r,
                                        pixelColor.g, pixelColor.b,
                                        pixelColor.a);
            SDL_RenderFillRect(mSDLRenderer, &pixel->getData());
        } else if (pixelState == PixelState::HOVER) {
            pixelColor = DEFAULT_COLORS.at(PixelColors::GREY);
            SDL_SetRenderDrawColorFloat(mSDLRenderer, pixelColor.r,
                                        pixelColor.g, pixelColor.b,
                                        pixelColor.a);
            SDL_RenderFillRect(mSDLRenderer, &pixel->getData());
        } else {
            pixelColor = pixel->getOutlineColor();
            SDL_SetRenderDrawColorFloat(mSDLRenderer, pixelColor.r,
                                        pixelColor.g, pixelColor.b, 0.5f);
            SDL_RenderRect(mSDLRenderer, &pixel->getData());
        };
    };
};

void PixelGrid::handleMouseEvent(SDL_Event *event, SDL_FColor &color) {
    static SDL_Point mousePos{0, 0};
    static int gridX{0};
    static int gridY{0};
    static int pixelIndex{0};

    mousePos = {(int)event->motion.x, (int)event->motion.y};

    if (!isWithinGrid(&mousePos, &mState->minimumPoint, &mState->maximumPoint))
        return;

    gridX = mousePos.x - mState->minimumPoint.x;
    gridX = std::floor(gridX / 25);

    gridY = mousePos.y - mState->minimumPoint.y;
    gridY = std::floor(gridY / 25);

    pixelIndex = gridX + (mState->gridWidth * gridY);
    // TODO: Somehow figure out how to have a "SELECTION" overlay on the current
    // pixel? maybe move around a seperate from the grid SDL_Rect that is half
    // transparent and grey? this way individual pixels don't store a seperate
    // bool
    if (event->type == SDL_EVENT_MOUSE_MOTION) {
        mPixels.at(pixelIndex)->handleMouseHover(event);
    } else if (SDL_EVENT_MOUSE_BUTTON_DOWN) {
        static const SDL_FRect &pix = mPixels.at(pixelIndex)->getData();
        mPixels.at(pixelIndex)->handleMouseClick(event, color);
    };
};

void PixelGrid::handleZoom(SDL_Event *event) {
    // ZOOOOOOmH
    return;
};

void PixelGrid::handleResize(SDL_Event *event) { return; };

}; // namespace HLX
