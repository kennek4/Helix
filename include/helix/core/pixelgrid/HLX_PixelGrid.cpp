#include "HLX_PixelGrid.h"
#include "HLX_EventSystem.h"
#include "HLX_Pixel.h"
#include "HLX_Window.h"

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
PixelGrid::PixelGrid(PixelGridState *state, SDLProps &sdlProps,
                     int &windowWidth, int &windowHeight)
    : mState(state), mSDLProps(&sdlProps) {

    SDL_Log("Reserving pixel amounts...");
    const int TOTAL_PIXEL_COUNT = mState->gridWidth * mState->gridHeight;
    mPixels.reserve(TOTAL_PIXEL_COUNT);

    SDL_Log("Setting Window WxH...");
    mState->windowWidth = windowWidth;
    mState->windowHeight = windowHeight;

    calculateBounds(windowWidth, windowHeight);
    registerCallbacks();
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

bool PixelGrid::init() {
    // NOTE: Set alpha blending mode
    SDL_SetRenderDrawBlendMode(mSDLProps->renderer, SDL_BLENDMODE_BLEND);

    const float sideLength = 25 * mState->currentZoom;

    // NOTE: Offset X, Y positions to account for SDL_Rect origin being top left
    // of the rectangle
    SDL_FRect *pixelData = new SDL_FRect;
    pixelData->x = mState->minimumPoint.x;
    pixelData->y = mState->minimumPoint.y;
    pixelData->w = pixelData->h = 25;

    mPixels.emplace_back(new Pixel(*pixelData));
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

    SDL_Log("Creating transparent background indicator...");
    // NOTE: Setup transparent background indicator(?)
    std::string bgPath = SDL_GetBasePath();
    SDL_Log("Checking for checkerboard.bmp at %s", bgPath.c_str());
    bgPath += "checkerboard.bmp";

    SDL_Surface *bgSurface = SDL_LoadBMP(bgPath.c_str());
    if (bgSurface == nullptr) {
        SDL_Log("Failed to load background: %s", SDL_GetError());
        return false;
    };

    mState->background =
        SDL_CreateTextureFromSurface(mSDLProps->renderer, bgSurface);
    SDL_SetTextureBlendMode(mState->background, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaModFloat(mState->background, 0.1f);
    SDL_DestroySurface(bgSurface); // Don't need surface anymore

    SDL_Log("Calculating backgroundTilingScale...");
    mState->backgroundTilingScale = (float)25 / 256;
    // (mState->background->w / (float)(mState->windowWidth)) / 4;

    SDL_Log("Setting backgroundRect...");
    mState->backgroundRect.x = (float)(mState->minimumPoint.x);
    mState->backgroundRect.y = (float)(mState->minimumPoint.y);
    mState->backgroundRect.w = (float)(mState->gridWidth * 25);
    mState->backgroundRect.h = (float)(mState->gridHeight * 25);
    return true;
};

void PixelGrid::reset() {
    if (mPixels.size() == 0)
        return;
    for (Pixel *pixel : mPixels) {
        pixel->setState(PixelState::EMPTY);
        pixel->setFillColor(DEFAULT_COLORS.at(PixelColors::WHITE));
    };
};

void PixelGrid::render() {
    static SDL_FColor pixelColor{255, 255, 255, SDL_ALPHA_OPAQUE};
    static PixelState pixelState{PixelState::EMPTY};

    SDL_RenderTextureTiled(mSDLProps->renderer, mState->background, NULL,
                           mState->backgroundTilingScale,
                           &mState->backgroundRect);

    // HACK: This shit looks wack, def better ways
    for (Pixel *pixel : mPixels) {
        pixelState = pixel->getState();
        if (pixelState == PixelState::FILLED) {
            pixelColor = pixel->getFillColor();
            SDL_SetRenderDrawColorFloat(mSDLProps->renderer, pixelColor.r,
                                        pixelColor.g, pixelColor.b,
                                        pixelColor.a);
            SDL_RenderFillRect(mSDLProps->renderer, &pixel->getData());
        } else if (pixelState == PixelState::HOVER) {
            pixelColor = DEFAULT_COLORS.at(PixelColors::GREY);
            SDL_SetRenderDrawColorFloat(mSDLProps->renderer, pixelColor.r,
                                        pixelColor.g, pixelColor.b,
                                        pixelColor.a);
            SDL_RenderFillRect(mSDLProps->renderer, &pixel->getData());
        } else {
            pixelColor = pixel->getOutlineColor();
            SDL_SetRenderDrawColorFloat(mSDLProps->renderer, pixelColor.r,
                                        pixelColor.g, pixelColor.b, 0.5f);
            SDL_RenderRect(mSDLProps->renderer, &pixel->getData());
        };
    };
};

void PixelGrid::saveImage() {
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

void PixelGrid::calculateBounds(int &newWidth, int &newHeight) {
    int *maxX = new int;
    int *maxY = new int;
    const SDL_DisplayMode *dm =
        SDL_GetDesktopDisplayMode(SDL_GetPrimaryDisplay());

    *maxX = dm->w;
    *maxY = dm->h;

    // NOTE: Calculate grid position
    mState->middlePoint = {newWidth / 2, newHeight / 2};
    SDL_Log("Mid Point: %d, %d", mState->middlePoint.x, mState->middlePoint.y);

    // NOTE: 25 would be the side length
    // NOTE: These point values are at the CENTRE of the Pixel,
    // this means that SDL_Rects are off by +25 on the X and Y axises
    mState->minimumPoint.x =
        mState->middlePoint.x - ((mState->gridWidth / 2) * 25);
    // mState->minimumPoint.x = std::clamp(mState->minimumPoint.x, 0, *maxX);

    mState->minimumPoint.y =
        mState->middlePoint.y - ((mState->gridHeight / 2) * 25);
    // mState->minimumPoint.y = std::clamp(mState->minimumPoint.y, 0, *maxY);

    SDL_Log("Min Point: %d, %d", mState->minimumPoint.x,
            mState->minimumPoint.y);

    mState->maximumPoint.x =
        mState->middlePoint.x + ((mState->gridWidth / 2) * 25);
    // mState->maximumPoint.x = std::clamp(mState->maximumPoint.x, 0, *maxY);

    mState->maximumPoint.y =
        mState->middlePoint.y + ((mState->gridHeight / 2) * 25);
    // mState->maximumPoint.y = std::clamp(mState->maximumPoint.y, 0, *maxY);
    SDL_Log("Max Point: %d, %d", mState->maximumPoint.x,
            mState->maximumPoint.y);

    delete maxX;
    delete maxY;
};

void PixelGrid::registerCallbacks() {
    auto handleWindowResize = [this](SDL_Event *event) -> void {
        mState->mouseScaleX =
            ((float)event->window.data1 / (float)mState->windowWidth);
        mState->mouseScaleY =
            ((float)event->window.data2 / (float)mState->windowHeight);

        calculateBounds(event->window.data1, event->window.data2);

        const float sideLength = 25 * mState->currentZoom;
        SDL_FRect *newPixelData = new SDL_FRect;
        newPixelData->x = mState->minimumPoint.x;
        newPixelData->y = mState->minimumPoint.y;
        newPixelData->w = newPixelData->h = sideLength;

        mPixels.at(0)->setData(*newPixelData);
        for (int y = 1; y < mPixels.capacity(); y++) {
            if (y % mState->gridHeight == 0) {
                newPixelData->x = mState->minimumPoint.x;
                newPixelData->y += sideLength;
            } else {
                newPixelData->x += sideLength;
            }

            mPixels.at(y)->setData(*newPixelData);
        };

        mState->backgroundRect.x = (float)(mState->minimumPoint.x);
        mState->backgroundRect.y = (float)(mState->minimumPoint.y);
        mState->backgroundRect.w = (float)(mState->gridWidth * 25);
        mState->backgroundRect.h = (float)(mState->gridHeight * 25);

        delete newPixelData;
    };

    auto handleMouseMotion = [this](SDL_Event *event) -> void {
        SDL_ConvertEventToRenderCoordinates(mSDLProps->renderer, event);
        mState->mousePos = {static_cast<int>(event->motion.x),
                            static_cast<int>(event->motion.y)};
    };

    auto handleBrushEvent = [this](SDL_Event *event) -> void {
        if (!isWithinGrid(&mState->mousePos, &mState->minimumPoint,
                          &mState->maximumPoint)) {
            return; // Mouse Position is out of grid bounds
        };

        int pixelIndex = getPixelIndex();
        const SDL_FRect &pix = mPixels.at(pixelIndex)->getData();
        static SDL_FColor *color =
            reinterpret_cast<SDL_FColor *>(event->user.data1);

        mPixels.at(pixelIndex)->handleMouseClick(event, color);
    };

    HLX::EventSystem::getInstance().subscribe(SDL_EVENT_WINDOW_RESIZED, this);
    mCallbackHandler.registerCallback(SDL_EVENT_WINDOW_RESIZED,
                                      handleWindowResize);

    HLX::EventSystem::getInstance().subscribe(SDL_EVENT_MOUSE_MOTION, this);
    mCallbackHandler.registerCallback(SDL_EVENT_MOUSE_MOTION,
                                      handleMouseMotion);

    HLX::EventSystem::getInstance().subscribe(SDL_EVENT_USER, this);
    mCallbackHandler.registerCallback(SDL_EVENT_USER, handleBrushEvent);
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
