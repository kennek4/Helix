#include "HLX_PixelGrid.h"
#include "HLX_EventSystem.h"
#include "HLX_Toolbox.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_stdinc.h>
#include <vector>

namespace {

inline bool isWithinGrid(const SDL_Point &point, const SDL_Point &minPoint,
                         const SDL_Point &maxPoint) {
    static bool inDomain{false};
    static bool inRange{false};

    inDomain = (point.x >= minPoint.x) && (point.x < maxPoint.x);
    inRange = (point.y >= minPoint.y) && (point.y < maxPoint.y);

    return inDomain && inRange;
};

inline bool isPixelEmpty(const std::vector<char> &PIXEL_STATES,
                         const int &PIXEL_INDEX) {
    return PIXEL_STATES.at(PIXEL_INDEX) != true;
};

inline bool isRGBAColorEqual(const SDL_FColor &COLOR_A,
                             const SDL_FColor &COLOR_B) {
    return ((COLOR_A.r == COLOR_B.r) && (COLOR_A.g == COLOR_B.g) &&
            (COLOR_A.b == COLOR_B.b) && (COLOR_A.a == COLOR_B.a));
};

inline int getPixelIndex(const int &x, const int &y, const SDL_Point &minPoint,
                         const int &gridWidthInPixels) {
    static int gridX{0};
    static int gridY{0};

    gridX = x - minPoint.x;
    gridX = std::floor(gridX / 25);

    gridY = y - minPoint.y;
    gridY = std::floor(gridY / 25);

    return gridX + (gridWidthInPixels * gridY);
};

void updateFRects(std::vector<SDL_FRect> &frects, const SDL_Point &MIN_POINT,
                  const float &SIDE_LENGTH, const int &GRID_HEIGHT) {

    SDL_FRect pixelData = {(float)MIN_POINT.x, (float)MIN_POINT.y, 25.0f,
                           25.0f};
    frects.at(0) = pixelData;

    for (int i = 1; i < frects.capacity(); i++) {
        if (i % GRID_HEIGHT == 0) {
            pixelData.x = MIN_POINT.x;
            pixelData.y += SIDE_LENGTH;
        } else {
            pixelData.x += SIDE_LENGTH;
        }

        frects.at(i) = pixelData;
    };
};
}; // namespace

namespace HLX {
PixelGrid::PixelGrid(SDLProps *sdlProps, const int GRID_WIDTH,
                     const int GRID_HEIGHT)
    : mSDLProps(sdlProps) {

    const int TOTAL_PIXEL_COUNT = mGrid.widthInPixels * mGrid.heightInPixels;
    SDL_Log("TOTAL_PIXEL_COUNT: %d", TOTAL_PIXEL_COUNT);

    SDL_Log("Reserving vector space...");
    mGrid.widthInPixels = GRID_WIDTH;
    mGrid.heightInPixels = GRID_HEIGHT;

    mGrid.frects.reserve(TOTAL_PIXEL_COUNT);
    // SDL_Log("mGrid.frects capacity: %d", mGrid.frects.capacity());
    mGrid.frects.assign(mGrid.frects.capacity(),
                        SDL_FRect{0.0f, 0.0f, 0.0f, 0.0f});
    // SDL_Log("mGrid.frects size: %d", mGrid.frects.size());

    mGrid.colors.reserve(TOTAL_PIXEL_COUNT);
    // SDL_Log("mGrid.colors capacity: %d", mGrid.colors.capacity());
    mGrid.colors.assign(
        mGrid.colors.capacity(),
        SDL_FColor{1.0f, 1.0f, 1.0f, SDL_ALPHA_TRANSPARENT_FLOAT});
    // SDL_Log("mGrid.colors size: %d", mGrid.colors.size());

    mGrid.states.reserve(TOTAL_PIXEL_COUNT);
    // SDL_Log("mGrid.states capacity: %d", mGrid.states.capacity());
    mGrid.states.assign(mGrid.states.capacity(), 0);
    // SDL_Log("mGrid.states size: %d", mGrid.states.size());
};

PixelGrid::~PixelGrid() {};

bool PixelGrid::onNotify(SDL_Event *event) {
    mCallbackHandler.invokeCallback(event);
    return true;
};

bool PixelGrid::init() {
    // NOTE: Set alpha blending mode
    SDL_SetRenderDrawBlendMode(mSDLProps->renderer, SDL_BLENDMODE_BLEND);

    SDL_Log("Setting Window WxH...");
    int windowWidth, windowHeight;

    SDL_GetWindowSize(mSDLProps->window, &windowWidth, &windowHeight);
    setGridBounds(windowWidth, windowHeight);

    registerWindowCallbacks();
    registerToolCallbacks();

    const float SIDE_LENGTH = 25 * 1.0f; // TODO: Update this ZoomLevel later
    updateFRects(mGrid.frects, mMinPoint, SIDE_LENGTH, mGrid.heightInPixels);

    // TODO: Update background FRect here
    mBackgroundFRect = {
        (float)mMinPoint.x,
        (float)mMinPoint.y,
        (float)mGrid.widthInPixels * 25,
        (float)mGrid.heightInPixels * 25,
    };

    return true;
};

void PixelGrid::reset() {
    if (mGrid.frects.empty()) {
        return;
    }

    for (int i = 0; i < (mGrid.widthInPixels * mGrid.heightInPixels); i++) {
        mGrid.colors.at(i) = {1.0f, 1.0f, 1.0f, SDL_ALPHA_TRANSPARENT_FLOAT};
    };
};

void PixelGrid::registerWindowCallbacks() {
    auto handleWindowResize = [this](SDL_Event *event) -> void {
        setGridBounds(event->window.data1, event->window.data2);

        // TODO: CHANGE THIS, ADD ZOOM LEVEL SOMEWHERE
        const float SIDE_LENGTH = 25 * 1.0f;

        updateFRects(mGrid.frects, mMinPoint, SIDE_LENGTH,
                     mGrid.heightInPixels);

        mBackgroundFRect = {
            (float)mMinPoint.x,
            (float)mMinPoint.y,
            (float)mGrid.widthInPixels * 25,
            (float)mGrid.heightInPixels * 25,
        };
    };

    HLX::EventSystem::getInstance().subscribe(SDL_EVENT_WINDOW_RESIZED, this);
    mCallbackHandler.registerCallback(SDL_EVENT_WINDOW_RESIZED,
                                      handleWindowResize);

    auto handleMouseMotion = [this](SDL_Event *event) -> void {
        SDL_ConvertEventToRenderCoordinates(mSDLProps->renderer, event);
        mMousePos = {(event->motion.x), (event->motion.y)};
    };

    HLX::EventSystem::getInstance().subscribe(SDL_EVENT_MOUSE_MOTION, this);
    mCallbackHandler.registerCallback(SDL_EVENT_MOUSE_MOTION,
                                      handleMouseMotion);
};

void PixelGrid::registerToolCallbacks() {
    static std::vector<int> brushIndicies;
    brushIndicies.reserve(16);

    const auto handleToolEvent = [this](SDL_Event *event) -> void {
        static SDL_Point startPoint;

        ToolProps *toolProps = static_cast<ToolProps *>(event->user.data1);

        startPoint = {(int)mMousePos.x, (int)mMousePos.y};
        if (!isWithinGrid(startPoint, mMinPoint, mMaxPoint)) {
            return;
        }

        switch (event->user.code) {
        case HELIX_EVENT_BRUSH:
            handleBrushEvent(startPoint, toolProps->color, toolProps->size,
                             true, brushIndicies);
            break;
        case HELIX_EVENT_ERASER:
            handleBrushEvent(
                startPoint,
                SDL_FColor{1.0f, 1.0f, 1.0f, SDL_ALPHA_TRANSPARENT_FLOAT},
                toolProps->size, false, brushIndicies);
            break;
        case HELIX_EVENT_BUCKET:
            handleBucketEvent(startPoint, toolProps->color);
            break;
        }
    };

    // BUG: HELIX_EVENT ID is +1 from what is created from
    // SDL_RegisterEvents(); How tf is it +1...?
    // SDL_Log("SUBSCRIBING TO EVENT ID: %d", HELIX_EVENT - 1);
    HLX::EventSystem::getInstance().subscribe(HELIX_EVENT - 1, this);
    mCallbackHandler.registerCallback(HELIX_EVENT - 1, handleToolEvent);
};

void PixelGrid::handleBrushEvent(const SDL_Point &startPoint,
                                 const SDL_FColor &brushColor,
                                 const int &brushSize, const bool isPixelActive,
                                 std::vector<int> &brushIndicies) {
    brushIndicies.clear();

    SDL_Point currentPoint;
    for (int i = 0; i < (brushSize * brushSize); i++) {
        const SDL_Point &offsets = SIZE_OFFSETS.at(i);
        currentPoint = {startPoint.x - offsets.x, startPoint.y - offsets.y};

        if (!isWithinGrid(currentPoint, mMinPoint, mMaxPoint)) {
            continue;
        };

        const int &pixelIndex = getPixelIndex(currentPoint.x, currentPoint.y,
                                              mMinPoint, mGrid.widthInPixels);
        brushIndicies.emplace_back(pixelIndex);
    };

    for (int &pointIndex : brushIndicies) {
        mGrid.states.at(pointIndex) = isPixelActive;
        mGrid.colors.at(pointIndex) = brushColor;
    };
};

/**
 * Iterative version of a flood fill algorithm. Uses a queue
 */
void PixelGrid::handleBucketEvent(const SDL_Point &startPoint,
                                  const SDL_FColor &bucketColor) {
    if (!isWithinGrid(startPoint, mMinPoint, mMaxPoint)) {
        return;
    };

    int pixelIndex = getPixelIndex(startPoint.x, startPoint.y, mMinPoint,
                                   mGrid.widthInPixels);

    const SDL_FColor originalColor = mGrid.colors.at(pixelIndex);

    if (isRGBAColorEqual(originalColor, bucketColor)) {
        return;
    };

    std::queue<SDL_Point> queue;
    SDL_Point seed = {startPoint.x, startPoint.y};
    queue.emplace(seed);

    SDL_Point currentPoint;
    SDL_FColor currentColor;

    while (!queue.empty()) {
        currentPoint = queue.front();
        queue.pop();

        if (!isWithinGrid(currentPoint, mMinPoint, mMaxPoint)) {
            continue;
        };

        pixelIndex = getPixelIndex(currentPoint.x, currentPoint.y, mMinPoint,
                                   mGrid.widthInPixels);

        currentColor = mGrid.colors.at(pixelIndex);

        if (!isRGBAColorEqual(originalColor, currentColor)) {
            continue;
        };

        mGrid.colors.at(pixelIndex) = bucketColor;
        mGrid.states.at(pixelIndex) = true;

        queue.emplace(SDL_Point{currentPoint.x - 25, currentPoint.y});
        queue.emplace(SDL_Point{currentPoint.x, currentPoint.y - 25});
        queue.emplace(SDL_Point{currentPoint.x + 25, currentPoint.y});
        queue.emplace(SDL_Point{currentPoint.x, currentPoint.y + 25});
    };
};

inline void PixelGrid::setGridBounds(const int newWindowWidth,
                                     const int newWindowHeight) {

    mMidPoint = {newWindowWidth / 2, newWindowHeight / 2};

    mMinPoint = {
        mMidPoint.x - ((mGrid.widthInPixels / 2) * 25),
        mMidPoint.y - ((mGrid.heightInPixels / 2) * 25),
    };

    mMaxPoint = {
        mMidPoint.x + ((mGrid.widthInPixels / 2) * 25),
        mMidPoint.y + ((mGrid.heightInPixels / 2) * 25),
    };
};
}; // namespace HLX
