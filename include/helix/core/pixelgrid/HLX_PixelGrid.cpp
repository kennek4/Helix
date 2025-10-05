#include "HLX_PixelGrid.h"
#include "HLX_EventSystem.h"
#include "HLX_Pixel.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

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

    HLX::EventSystem::getInstance().subscribe(SDL_EVENT_WINDOW_RESIZED, this);
    mCallbackHandler.registerCallback(
        SDL_EVENT_WINDOW_RESIZED,
        [this](SDL_Event *event) -> void { std::println("Window Resized!"); });

    HLX::EventSystem::getInstance().subscribe(SDL_EVENT_MOUSE_MOTION, this);
    mCallbackHandler.registerCallback(
        SDL_EVENT_MOUSE_MOTION, [this](SDL_Event *event) -> void {
            mState->mousePos = {(int)event->motion.x, (int)event->motion.y};
        });

    HLX::EventSystem::getInstance().subscribe(SDL_EVENT_USER, this);
    mCallbackHandler.registerCallback(
        SDL_EVENT_USER, [this](SDL_Event *event) -> void {
            if (!isWithinGrid(&mState->mousePos, &mState->minimumPoint,
                              &mState->maximumPoint)) {

                return;
            };

            int pixelIndex = getPixelIndex();
            const SDL_FRect &pix = mPixels.at(pixelIndex)->getData();
            static SDL_FColor *color =
                reinterpret_cast<SDL_FColor *>(event->user.data1);

            mPixels.at(pixelIndex)->handleMouseClick(event, color);
        });
};

PixelGrid::~PixelGrid() {
    for (Pixel *pixel : mPixels) {
        delete pixel;
    };
};

bool PixelGrid::onNotify(SDL_Event *event) {
    mCallbackHandler.invokeCallback(event);
    return true;
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

void PixelGrid::saveToSurface(SDL_Surface &surface) {
    Uint8 *pixels = new Uint8[mPixels.size() * STBI_rgb_alpha]();

    int r, g, b, a;

    int index = 0;
    for (int pos = 0; pos < mPixels.capacity(); pos++) {
        const SDL_FColor fillColor = mPixels.at(pos)->getFillColor();
        pixels[index++] = (int)(fillColor.r * 255.99f);
        pixels[index++] = (int)(fillColor.g * 255.99f);
        pixels[index++] = (int)(fillColor.b * 255.99f);
        pixels[index++] = (int)(fillColor.a * 255.99f);
    };

    SDL_Log("Saving Image!");
    std::string path = SDL_GetBasePath();
    path += "SavedImage.png";

    int success = stbi_write_png(path.c_str(), mState->gridWidth,
                                 mState->gridWidth, STBI_rgb_alpha, pixels,
                                 mState->gridWidth * STBI_rgb_alpha);

    if (success == 0) {
        SDL_Log("Saving image failed...");
    } else {
        SDL_Log("Saving image success!");
    };

    delete[] pixels;
};

int PixelGrid::getPixelIndex() {
    static int gridX{0};
    static int gridY{0};

    gridX = mState->mousePos.x - mState->minimumPoint.x;
    gridX = std::floor(gridX / 25);

    gridY = mState->mousePos.y - mState->minimumPoint.y;
    gridY = std::floor(gridY / 25);

    return gridX + (mState->gridWidth * gridY);
};

}; // namespace HLX
