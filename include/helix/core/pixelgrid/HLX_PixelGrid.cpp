#include "HLX_PixelGrid.h"
#include "HLX_Constants.h"

namespace {
inline bool isRGBAColorEqual(const SDL_FColor &colorA,
                             const SDL_FColor &colorB) {
    return ((colorA.r == colorB.r) && (colorA.g == colorB.g) &&
            (colorA.b == colorB.b) && (colorA.a == colorB.a));
};

inline int getPixelIndex(const int &x, const int &y, const SDL_Point &minPoint,
                         const int &gridWidthInPixels) {
    return std::floor((x - minPoint.x) / 25) +
           (gridWidthInPixels * std::floor((y - minPoint.y) / 25));
};

void updateFRects(std::vector<SDL_FRect> &frects, const SDL_Point &minPoint,
                  const float &sideLength, const int &gridHeight) {

    SDL_FRect pixelData = {(float)minPoint.x, (float)minPoint.y, 25.0f, 25.0f};
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

    const int &totalPixelCount = mGrid.widthInPixels * mGrid.heightInPixels;
    SDL_Log("totalPixelCount: %d", totalPixelCount);

    SDL_Log("Reserving vector space...");
    mGrid.widthInPixels = widthInPixels;
    mGrid.heightInPixels = heightInPixels;

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

    const float sideLength = 25 * 1.0f; // TODO: Update this ZoomLevel later
    updateFRects(mGrid.frects, mMinPoint, sideLength, mGrid.heightInPixels);

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
        const float sideLength = 25 * 1.0f;
        updateFRects(mGrid.frects, mMinPoint, sideLength, mGrid.heightInPixels);

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
        if (!isPointInGrid(startPoint)) {
            return;
        }

        switch (event->user.code) {
        case Constants::HelixEventToolBrush:
            handleBrushEvent(startPoint, toolProps->color, toolProps->size,
                             Constants::PixelStateFilled, brushIndicies);
            break;
        case Constants::HelixEventToolEraser:
            handleBrushEvent(startPoint, Constants::EmptyPixelFColor,
                             toolProps->size, Constants::PixelStateEmpty,
                             brushIndicies);
            break;
        case Constants::HelixEventToolBucket:
            handleBucketEvent(startPoint, toolProps->color);
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
        const SDL_Point &offsets = Constants::BrushSizePointOffsets[i];
        currentPoint = {startPoint.x - offsets.x, startPoint.y - offsets.y};

        if (!isPointInGrid(currentPoint)) {
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
    if (!isPointInGrid(startPoint)) {
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

        if (!isPointInGrid(currentPoint)) {
            continue;
        };

        pixelIndex = getPixelIndex(currentPoint.x, currentPoint.y, mMinPoint,
                                   mGrid.widthInPixels);

        currentColor = mGrid.colors[pixelIndex];

        if (!isRGBAColorEqual(originalColor, currentColor)) {
            continue;
        };

        mGrid.colors[pixelIndex] = bucketColor;
        mGrid.states[pixelIndex] = Constants::PixelStateFilled;

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

inline bool PixelGrid::isPointInGrid(const SDL_Point &point) {
    return ((point.x >= mMinPoint.x) && (point.x < mMaxPoint.x)) &&
           ((point.y >= mMinPoint.y) && (point.y < mMaxPoint.y));
};

inline bool PixelGrid::isPixelEmpty(const int &pixelIndex) {
    return mGrid.states.at(pixelIndex) != true;
};

}; // namespace HLX
