#include "HLX_PixelGrid.h"
#include "HLX_Constants.h"
#include <SDL3/SDL_log.h>
#include <algorithm>

namespace {
inline bool isRGBAColorEqual(const SDL_FColor &colorA,
                             const SDL_FColor &colorB) {
    return ((colorA.r == colorB.r) && (colorA.g == colorB.g) &&
            (colorA.b == colorB.b) && (colorA.a == colorB.a));
};

inline int getPixelIndex(const int &x, const int &y, const SDL_Point &minPoint,
                         const int &gridWidthInPixels,
                         const float &sideLength) {
    return std::floor((x - minPoint.x) / sideLength) +
           (gridWidthInPixels * std::floor((y - minPoint.y) / sideLength));
};

void updateFRects(std::vector<SDL_FRect> &frects, const SDL_Point &minPoint,
                  const float &sideLength, const int &gridHeight) {

    SDL_FRect pixelData = {(float)minPoint.x, (float)minPoint.y, sideLength,
                           sideLength};
    frects[0] = pixelData;

    for (int i = 1; i < frects.size(); i++) {
        if (i % gridHeight == 0) {
            pixelData.x = minPoint.x;
            pixelData.y += sideLength;
        } else {
            pixelData.x += sideLength;
        }

        frects[i] = pixelData;
    };
};

}; // namespace

namespace HLX {
PixelGrid::PixelGrid(SDLProps *sdlProps, const int widthInPixels,
                     const int heightInPixels)
    : mSDLProps(sdlProps) {

    mGrid.widthInPixels = widthInPixels;
    mGrid.heightInPixels = heightInPixels;
    mGrid.pixelSideLength =
        Constants::GridSizeToPixelSideLength[mGrid.widthInPixels];

    const int &totalPixelCount = mGrid.widthInPixels * mGrid.heightInPixels;
    SDL_Log("totalPixelCount: %d", totalPixelCount);

    SDL_Log("Reserving vector space...");

    mGrid.frects.reserve(totalPixelCount);
    mGrid.frects.assign(totalPixelCount, SDL_FRect{0.0f, 0.0f, 0.0f, 0.0f});

    mGrid.colors.reserve(totalPixelCount);
    mGrid.colors.assign(totalPixelCount, Constants::EmptyPixelFColor);

    mGrid.states.reserve(totalPixelCount);
    mGrid.states.assign(totalPixelCount, Constants::PixelStateEmpty);
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

    const float sideLength =
        mGrid.pixelSideLength * 1.0f; // TODO: Update this ZoomLevel later
    updateFRects(mGrid.frects, mMinPoint, mGrid.pixelSideLength,
                 mGrid.heightInPixels);

    mBackgroundFRect = {
        (float)mMinPoint.x,
        (float)mMinPoint.y,
        (float)mGrid.widthInPixels * mGrid.pixelSideLength,
        (float)mGrid.heightInPixels * mGrid.pixelSideLength,
    };

    return true;
};

void PixelGrid::reset() {
    if (mGrid.frects.empty()) {
        return;
    }

    const int &numOfPixels = mGrid.widthInPixels * mGrid.heightInPixels;
    mGrid.states.assign(numOfPixels, Constants::PixelStateEmpty);
    mGrid.colors.assign(numOfPixels, Constants::EmptyPixelFColor);

    for (int i = 0; i < (mGrid.widthInPixels * mGrid.heightInPixels); i++) {
        mGrid.colors.at(i) = {1.0f, 1.0f, 1.0f, SDL_ALPHA_TRANSPARENT_FLOAT};
    };
};

void PixelGrid::registerWindowCallbacks() {
    auto handleWindowResize = [this](SDL_Event *event) -> void {
        setGridBounds(event->window.data1, event->window.data2);

        // TODO: CHANGE THIS, ADD ZOOM LEVEL SOMEWHERE
        const float sideLength = mGrid.pixelSideLength * 1.0f;
        updateFRects(mGrid.frects, mMinPoint, sideLength, mGrid.heightInPixels);

        mBackgroundFRect = {
            (float)mMinPoint.x,
            (float)mMinPoint.y,
            (float)mGrid.widthInPixels * mGrid.pixelSideLength,
            (float)mGrid.heightInPixels * mGrid.pixelSideLength,
        };
    };

    HLX::EventSystem::getInstance().subscribe(SDL_EVENT_WINDOW_RESIZED, this);
    mCallbackHandler.registerCallback(SDL_EVENT_WINDOW_RESIZED,
                                      handleWindowResize);

    auto handleMouseMotion = [this](SDL_Event *event) -> void {
        SDL_ConvertEventToRenderCoordinates(mSDLProps->renderer, event);
        mMousePos = {event->motion.x, event->motion.y};
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

        switch (event->user.code) {
        case Constants::EventGUIHasPriority:
            mIsActive = false;
            break;
        case Constants::EventGUINoPriority:
            mIsActive = true;
            break;
        }

        if (!mIsActive)
            return;

        ToolProps *toolProps = static_cast<ToolProps *>(event->user.data1);

        startPoint = {(int)mMousePos.x, (int)mMousePos.y};
        if (!isPointInGrid(startPoint)) {
            return;
        }

        switch (event->user.code) {
        case Constants::HelixEventToolBrush:
            handleBrushEvent(startPoint, *toolProps->currentColor,
                             toolProps->size, Constants::PixelStateFilled,
                             brushIndicies);
            break;
        case Constants::HelixEventToolEraser:
            handleBrushEvent(startPoint, Constants::EmptyPixelFColor,
                             toolProps->size, Constants::PixelStateEmpty,
                             brushIndicies);
            break;
        case Constants::HelixEventToolBucket:
            handleBucketEvent(startPoint, *toolProps->currentColor);
            break;
        }
    };

    HLX::EventSystem::getInstance().subscribe(Constants::HelixEvent, this);
    mCallbackHandler.registerCallback(Constants::HelixEvent, handleToolEvent);
};

void PixelGrid::handleBrushEvent(const SDL_Point &startPoint,
                                 const SDL_FColor &brushColor,
                                 const int &brushSize, const bool isPixelActive,
                                 std::vector<int> &brushIndicies) {
    brushIndicies.clear();
    SDL_Point currentPoint;

    for (int i = 0; i < (brushSize * brushSize); i++) {
        const SDL_Point &offset = Constants::BrushSizePointOffsets[i];
        currentPoint = {startPoint.x - (offset.x * mGrid.pixelSideLength),
                        startPoint.y - (offset.y * mGrid.pixelSideLength)};

        if (!isPointInGrid(currentPoint)) {
            continue;
        };

        const int &pixelIndex =
            getPixelIndex(currentPoint.x, currentPoint.y, mMinPoint,
                          mGrid.widthInPixels, mGrid.pixelSideLength);
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
    if (!isPointInGrid(startPoint)) {
        return;
    };

    int pixelIndex = getPixelIndex(startPoint.x, startPoint.y, mMinPoint,
                                   mGrid.widthInPixels, mGrid.pixelSideLength);
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

        if (!isPointInGrid(currentPoint)) {
            continue;
        };

        pixelIndex = getPixelIndex(currentPoint.x, currentPoint.y, mMinPoint,
                                   mGrid.widthInPixels, mGrid.pixelSideLength);

        currentColor = mGrid.colors[pixelIndex];

        if (!isRGBAColorEqual(originalColor, currentColor)) {
            continue;
        };

        mGrid.colors[pixelIndex] = bucketColor;
        mGrid.states[pixelIndex] = Constants::PixelStateFilled;

        queue.emplace(
            SDL_Point{currentPoint.x - mGrid.pixelSideLength, currentPoint.y});
        queue.emplace(
            SDL_Point{currentPoint.x, currentPoint.y - mGrid.pixelSideLength});
        queue.emplace(
            SDL_Point{currentPoint.x + mGrid.pixelSideLength, currentPoint.y});
        queue.emplace(
            SDL_Point{currentPoint.x, currentPoint.y + mGrid.pixelSideLength});
    };
};

inline void PixelGrid::setGridBounds(const int newWindowWidth,
                                     const int newWindowHeight) {

    mMidPoint = {newWindowWidth / 2, newWindowHeight / 2};

    mMinPoint = {
        mMidPoint.x - ((mGrid.widthInPixels / 2) * mGrid.pixelSideLength),
        mMidPoint.y - ((mGrid.heightInPixels / 2) * mGrid.pixelSideLength),
    };

    mMaxPoint = {
        mMidPoint.x + ((mGrid.widthInPixels / 2) * mGrid.pixelSideLength),
        mMidPoint.y + ((mGrid.heightInPixels / 2) * mGrid.pixelSideLength),
    };
};

inline bool PixelGrid::isPointInGrid(const SDL_Point &point) {
    const bool &inGrid =
        ((point.x >= mMinPoint.x) && (point.x < mMaxPoint.x)) &&
        ((point.y >= mMinPoint.y) && (point.y < mMaxPoint.y));

    return inGrid;
};

inline bool PixelGrid::isPixelEmpty(const int &pixelIndex) {
    return mGrid.states.at(pixelIndex) != true;
};

}; // namespace HLX
