#include "HLX_PixelGrid.h"
#include <vector>

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

inline bool isPixelEmpty(const std::vector<char> &PIXEL_STATES,
                         const int &PIXEL_INDEX) {
    return PIXEL_STATES.at(PIXEL_INDEX) != true;
};

namespace HLX {
PixelGrid::PixelGrid(PixelGridState *state, SDLProps &sdlProps)
    : mState(state), mSDLProps(&sdlProps) {
    SDL_Log("Reserving pixel amounts...");
    const int TOTAL_PIXEL_COUNT = mState->gridWidth * mState->gridHeight;

    mPixels.data.reserve(TOTAL_PIXEL_COUNT);

    mPixels.color.reserve(TOTAL_PIXEL_COUNT);
    mPixels.color.assign(
        mPixels.color.capacity(),
        SDL_FColor{1.0f, 1.0f, 1.0f, SDL_ALPHA_TRANSPARENT_FLOAT});

    mPixels.state.reserve(TOTAL_PIXEL_COUNT);
    mPixels.state.assign(mPixels.state.capacity(), 0);

    SDL_Log("Setting Window WxH...");

    int *winWidth = new int;
    int *winHeight = new int;

    SDL_GetWindowSize(mSDLProps->window, winWidth, winHeight);
    calculateBounds(*winWidth, *winHeight);
    delete winWidth;
    delete winHeight;

    registerWindowCallbacks();
    registerToolCallbacks();
};

PixelGrid::~PixelGrid() {};

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

    mPixels.data.emplace_back(*pixelData);

    for (int y = 1; y < mPixels.data.capacity(); y++) {
        if (y % mState->gridHeight == 0) {
            pixelData->x = mState->minimumPoint.x;
            pixelData->y += sideLength;
        } else {
            pixelData->x += sideLength;
        }

        mPixels.data.emplace_back(*pixelData);
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
    SDL_SetTextureAlphaModFloat(mState->background, 0.05f);
    SDL_DestroySurface(bgSurface); // Don't need surface anymore

    SDL_Log("Calculating backgroundTilingScale...");
    mState->backgroundTilingScale = (float)25 / 256;

    SDL_Log("Setting backgroundRect...");
    mState->backgroundRect.x = (float)(mState->minimumPoint.x);
    mState->backgroundRect.y = (float)(mState->minimumPoint.y);
    mState->backgroundRect.w = (float)(mState->gridWidth * 25);
    mState->backgroundRect.h = (float)(mState->gridHeight * 25);
    return true;
};

void PixelGrid::reset() {
    if (mPixels.data.size() != 0) {
        for (int i = 0; i < (mState->gridWidth * mState->gridHeight); i++) {
            mPixels.color.at(i) = {1.0f, 1.0f, 1.0f,
                                   SDL_ALPHA_TRANSPARENT_FLOAT};
        };
    };
};

void PixelGrid::render() {
    static SDL_FColor pixelColor{255, 255, 255, SDL_ALPHA_OPAQUE};

    SDL_RenderTextureTiled(mSDLProps->renderer, mState->background, NULL,
                           mState->backgroundTilingScale,
                           &mState->backgroundRect);

    for (int i = 0; i < (mState->gridWidth * mState->gridHeight); i++) {
        pixelColor = mPixels.color.at(i);

        if (!isPixelEmpty(mPixels.state, i)) {
            SDL_SetRenderDrawColorFloat(mSDLProps->renderer, pixelColor.r,
                                        pixelColor.g, pixelColor.b,
                                        pixelColor.a);
            SDL_RenderFillRect(mSDLProps->renderer, &mPixels.data.at(i));
        } else {
            SDL_SetRenderDrawColorFloat(mSDLProps->renderer, pixelColor.r,
                                        pixelColor.g, pixelColor.b, 0.2f);
            SDL_RenderRect(mSDLProps->renderer, &mPixels.data.at(i));
        };
    };
};

void PixelGrid::saveImage() {
    // Uint8 *pixels = new Uint8[mPixels.size() * STBI_rgb_alpha]();
    //
    // int r, g, b, a;
    //
    // int index = 0;
    // for (int pos = 0; pos < mPixels.capacity(); pos++) {
    //     const SDL_FColor fillColor = mPixels.at(pos)->getFillColor();
    //     pixels[index++] = (int)(fillColor.r * 255.99f);
    //     pixels[index++] = (int)(fillColor.g * 255.99f);
    //     pixels[index++] = (int)(fillColor.b * 255.99f);
    //     pixels[index++] = (int)(fillColor.a * 255.99f);
    // };
    //
    // SDL_Log("Saving Image!");
    // std::string path = SDL_GetBasePath();
    // path += "SavedImage.png";
    //
    // int success = stbi_write_png(path.c_str(), mState->gridWidth,
    //                              mState->gridWidth, STBI_rgb_alpha, pixels,
    //                              mState->gridWidth * STBI_rgb_alpha);
    //
    // if (success == 0) {
    //     SDL_Log("Saving image failed...");
    // } else {
    //     SDL_Log("Saving image success!");
    // };
    //
    // delete[] pixels;
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

    delete maxX;
    delete maxY;
};

void PixelGrid::registerWindowCallbacks() {
    auto handleWindowResize = [this](SDL_Event *event) -> void {
        calculateBounds(event->window.data1, event->window.data2);

        const float sideLength = 25 * mState->currentZoom;
        SDL_FRect *newPixelData = new SDL_FRect;
        newPixelData->x = mState->minimumPoint.x;
        newPixelData->y = mState->minimumPoint.y;
        newPixelData->w = newPixelData->h = sideLength;

        mPixels.data.at(0) = *newPixelData;
        for (int i = 1; i < mPixels.data.size(); i++) {
            if (i % mState->gridHeight == 0) {
                newPixelData->x = mState->minimumPoint.x;
                newPixelData->y += sideLength;
            } else {
                newPixelData->x += sideLength;
            }

            mPixels.data.at(i) = *newPixelData;
        };

        mState->backgroundRect.x = (float)(mState->minimumPoint.x);
        mState->backgroundRect.y = (float)(mState->minimumPoint.y);
        mState->backgroundRect.w = (float)(mState->gridWidth * 25);
        mState->backgroundRect.h = (float)(mState->gridHeight * 25);

        delete newPixelData;
    };

    auto handleMouseMotion = [this](SDL_Event *event) -> void {
        SDL_ConvertEventToRenderCoordinates(mSDLProps->renderer, event);
        mState->mousePos = {(int)(event->motion.x), (int)(event->motion.y)};
    };

    HLX::EventSystem::getInstance().subscribe(SDL_EVENT_WINDOW_RESIZED, this);
    mCallbackHandler.registerCallback(SDL_EVENT_WINDOW_RESIZED,
                                      handleWindowResize);

    HLX::EventSystem::getInstance().subscribe(SDL_EVENT_MOUSE_MOTION, this);
    mCallbackHandler.registerCallback(SDL_EVENT_MOUSE_MOTION,
                                      handleMouseMotion);
};

void PixelGrid::registerToolCallbacks() {
    static std::vector<int> brushIndicies;
    brushIndicies.reserve(16);

    const auto handleToolEvent = [this](SDL_Event *event) -> void {
        static bool isActive;
        static SDL_FColor color;
        static SDL_Point startPoint;

        brushIndicies.clear();

        ToolProps *toolProps = static_cast<ToolProps *>(event->user.data1);
        startPoint = {mState->mousePos.x, mState->mousePos.y};

        SDL_Point currentPoint;

        for (int i = 0; i < (toolProps->size * toolProps->size); i++) {
            const SDL_Point &offsets = SIZE_OFFSETS.at(i);
            currentPoint = {startPoint.x - offsets.x, startPoint.y - offsets.y};

            if (!isWithinGrid(&currentPoint, &mState->minimumPoint,
                              &mState->maximumPoint)) {
                continue;
            };

            brushIndicies.emplace_back(
                getPixelIndex(currentPoint.x, currentPoint.y));
        };

        if (event->user.code == HELIX_EVENT_BRUSH) {
            color = toolProps->color;
            isActive = true;
        } else if (event->user.code == HELIX_EVENT_ERASER) {
            color = {1.0f, 1.0f, 1.0f, SDL_ALPHA_TRANSPARENT_FLOAT};
            isActive = false;
        };

        for (int &pointIndex : brushIndicies) {
            mPixels.state.at(pointIndex) = isActive;
            mPixels.color.at(pointIndex) = color;
        };
    };

    // BUG: HELIX_EVENT ID is +1 from what is created from SDL_RegisterEvents();
    // How tf is it +1...?

    // SDL_Log("SUBSCRIBING TO EVENT ID: %d", HELIX_EVENT - 1);
    HLX::EventSystem::getInstance().subscribe(HELIX_EVENT - 1, this);
    mCallbackHandler.registerCallback(HELIX_EVENT - 1, handleToolEvent);
};

inline int PixelGrid::getPixelIndex(int &xPos, int &yPos) {
    static int gridX{0};
    static int gridY{0};

    gridX = xPos - mState->minimumPoint.x;
    gridX = std::floor(gridX / 25);

    gridY = yPos - mState->minimumPoint.y;
    gridY = std::floor(gridY / 25);

    return gridX + (mState->gridWidth * gridY);
};

}; // namespace HLX
