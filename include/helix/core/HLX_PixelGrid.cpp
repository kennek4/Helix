#include "HLX_PixelGrid.h"
#include "HLX_Pixel.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <algorithm>

namespace HLX {

PixelGrid::PixelGrid(PixelGridProperties props, SDL_Renderer *renderer)
    : mProps(props), mSDLRenderer(renderer) {
    const int TOTAL_PIXEL_COUNT = mProps.gridWidth * mProps.gridHeight;
    mPixels.reserve(TOTAL_PIXEL_COUNT);

    // Create default initial grid
    const float sideLength = 25 * mProps.currentZoom;

    SDL_FRect pixelData;
    pixelData.x = mProps.gridStartPoint.x;
    pixelData.y = mProps.gridStartPoint.y;
    pixelData.w = pixelData.h = 25;

    for (int y = 0; y < mPixels.capacity(); y++) {
        if (y % mProps.gridHeight == 0) {
            pixelData.x = mProps.gridStartPoint.x;
            pixelData.y += sideLength;
        };

        pixelData.x += sideLength;
        mPixels.emplace_back(new Pixel(pixelData));
    };

    // TODO: Replace 25 with pixel side length
    // 100 is grid offset
    mProps.canvasMaxY =
        mProps.gridStartPoint.y + (mProps.gridHeight * 25) + 100;
    mProps.canvasMaxX = mProps.gridStartPoint.x + (mProps.gridWidth * 25) + 100;
    SDL_Log("Max X: %d, Max Y: %d", mProps.canvasMaxX, mProps.canvasMaxY);
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
    SDL_Color pixelColor;
    PixelState pixelState;

    for (Pixel *pixel : mPixels) {
        pixelState = pixel->getState();

        if (pixelState == PixelState::FILLED) {
            pixelColor = pixel->getFillColor();
            SDL_SetRenderDrawColor(mSDLRenderer, pixelColor.r, pixelColor.g,
                                   pixelColor.b, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(mSDLRenderer, &pixel->getData());
        } else if (pixelState == PixelState::HOVER) {
            pixelColor = DEFAULT_COLORS.at(PixelColors::GREY);
            SDL_SetRenderDrawColor(mSDLRenderer, pixelColor.r, pixelColor.g,
                                   pixelColor.b, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(mSDLRenderer, &pixel->getData());
        } else {
            pixelColor = pixel->getOutlineColor();
            SDL_SetRenderDrawColor(mSDLRenderer, pixelColor.r, pixelColor.g,
                                   pixelColor.b, SDL_ALPHA_OPAQUE);
            SDL_RenderRect(mSDLRenderer, &pixel->getData());
        };
    };
};

void PixelGrid::handleMouseEvent(SDL_Event *event) {
    static SDL_FPoint mousePos{0, 0};
    mousePos = {event->motion.x, event->motion.y};

    // TODO: Add canvas bounds checking here
    //  check if the mouse click/motion is even within the canvas
    //  if not, discard / do nothing
    //  100 is offset
    static bool inDomain{false};
    static bool inRange{false};

    // TODO: 125 is offset + sideLength
    inDomain = mousePos.x >= 125 && mousePos.x <= mProps.canvasMaxX;
    // SDL_Log("inDomain: %b", inDomain);

    inRange = mousePos.y >= 125 && mousePos.y <= mProps.canvasMaxY;
    // SDL_Log("inRange: %b", inRange);

    if (!(inDomain && inRange)) {
        return;
    }

    static int gridX{0};
    static int gridY{0};

    // TODO: Replace 25 with the Pixel "side length"
    // 100 is the offset of the canvas
    gridX = (int)mousePos.x - 100;
    gridX = std::max(0, gridX);
    gridX = gridX / 25 - 1;

    gridY = (int)mousePos.y - 100;
    gridY = std::max(0, gridY);
    gridY = gridY / 25 - 1;

    // i = (gridX + (gridWidth * gridY) - gridWidth + 1) -
    static int pixelIndex{0};

    // TODO: 32 is the gridWidth
    // TODO: 100 is the offset of grid
    pixelIndex = gridX + ((32 * gridY));

    if (!SDL_PointInRectFloat(&mousePos, &mPixels.at(pixelIndex)->getData())) {
    };

    // mLastUsedPixel = &mPixels.at(pixelIndex);

    if (event->type == SDL_EVENT_MOUSE_MOTION) {
        mPixels.at(pixelIndex)->handleMouseHover(event);
    } else if (SDL_EVENT_MOUSE_BUTTON_DOWN) {
        const SDL_FRect &pix = mPixels.at(pixelIndex)->getData();
        SDL_Log("mx: %f, my: %f", event->motion.x, event->motion.y);
        SDL_Log("px: %f, py: %f", pix.x, pix.y);
        mPixels.at(pixelIndex)
            ->handleMouseClick(event, SDL_Color{255, 0, 0, SDL_ALPHA_OPAQUE});
    };
};

void PixelGrid::handleZoom(SDL_Event *event) {
    // ZOOOOOOmH
    return;
};

void PixelGrid::handleResize(SDL_Event *event) { return; };

}; // namespace HLX
