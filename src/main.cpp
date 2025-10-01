#include "HLX_Gui.h"
#include "Helix.h"
#include "pixelgrid/HLX_PixelGrid.h"
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <string>

#define SDL_MAIN_USE_CALLBACKS
#include "SDL3/SDL_main.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

static bool isMouseDown = false;

constexpr SDL_InitFlags initFlags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;
constexpr SDL_WindowFlags winFlags =
    SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIGH_PIXEL_DENSITY |
    SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_FULLSCREEN;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    SDL_Log("Initializing Helix...");

    *appstate = new HLX::HelixState;
    HLX::HelixState &helix = *static_cast<HLX::HelixState *>(*appstate);

    SDL_Log("Creating new Window/Renderer...");
    if (!SDL_CreateWindowAndRenderer("Helix Sprite Maker", 1600, 900, winFlags,
                                     &helix.window, &helix.renderer)) {
        SDL_Log("[FAIL] SDL failed to create Window/Renderer: %s",
                SDL_GetError());
        return SDL_APP_FAILURE;
    };

    SDL_SetWindowFullscreen(helix.window, true);

    HLX::GUI::init(helix.renderer, helix.window);

    SDL_GetWindowSize(helix.window, &helix.windowWidth, &helix.windowHeight);
    SDL_Log("Window W: %d, Window Height :%d", helix.windowWidth,
            helix.windowHeight);

    SDL_Log("Creating transparent background indicator...");
    // NOTE: Setup transparent background indicator(?)
    std::string bgPath = SDL_GetBasePath();
    bgPath += "checkerboard.bmp";

    SDL_Surface *bgSurface = SDL_LoadBMP(bgPath.c_str());
    if (bgSurface == nullptr) {
        SDL_Log("Failed to load background: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    };

    helix.background = SDL_CreateTextureFromSurface(helix.renderer, bgSurface);
    SDL_SetTextureBlendMode(helix.background, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaModFloat(helix.background, 0.1f);
    SDL_DestroySurface(bgSurface);

    helix.backgroundTilingScale =
        (helix.background->w / (float)helix.windowWidth) / 4;

    // NOTE: 25 is the length of a pixel side, mid point x/y - 25 accounts for
    // how SDL_Rects start drawing at the top left corner
    // props.gridMidPoint = SDL_Point{(w / 2) - 25, (h / 2) - 25};
    helix.pixelGridState.gridWidth = 32;
    helix.pixelGridState.gridHeight = 32;
    helix.pixelGrid = new HLX::PixelGrid(&helix.pixelGridState, helix.renderer,
                                         helix.windowWidth, helix.windowHeight);

    SDL_Log("Setting backgroundRect...");
    helix.backgroundRect.x = (float)helix.pixelGridState.minimumPoint.x;
    helix.backgroundRect.y = (float)helix.pixelGridState.minimumPoint.y;
    helix.backgroundRect.w = (float)helix.pixelGridState.gridWidth * 25;
    helix.backgroundRect.h = (float)helix.pixelGridState.gridHeight * 25;

    return SDL_APP_CONTINUE;
};

// Main Application Loop
SDL_AppResult SDL_AppIterate(void *appstate) {
    HLX::HelixState &helix = *static_cast<HLX::HelixState *>(appstate);

    // Process Input
    // NOTE: This allows mouse painting / dragging
    if (isMouseDown) {
        SDL_Event mouseDragEvent;
        float x, y;

        SDL_zero(mouseDragEvent);
        mouseDragEvent.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
        SDL_GetMouseState(&x, &y);
        mouseDragEvent.motion.x = x;
        mouseDragEvent.motion.y = y;

        SDL_PushEvent(&mouseDragEvent);
    };

    // helixPixelGrid->handleMouseDrag(isMouseDown);

    // NOTE: Clear screen
    SDL_SetRenderDrawColor(helix.renderer, 255, 255, 255,
                           SDL_ALPHA_OPAQUE); /* black, full alpha */
    SDL_RenderClear(helix.renderer);          /* start with a blank canvas. */
    SDL_RenderTextureTiled(helix.renderer, helix.background, NULL,
                           helix.backgroundTilingScale, &helix.backgroundRect);

    HLX::GUI::newFrame();
    HLX::GUI::renderToolbar();
    HLX::GUI::renderPalette(helix.brush.rawColor, helix.brush.fillColor);

    helix.pixelGrid->render();

    HLX::GUI::renderFrame(helix.renderer);
    SDL_RenderPresent(helix.renderer); /* put it all on the screen! */
    return SDL_APP_CONTINUE;           /* carry on with the program! */
};

// Event Handling
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    HLX::HelixState &helix = *static_cast<HLX::HelixState *>(appstate);
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        isMouseDown = true;
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        isMouseDown = false;
    };

    switch (event->type) {
    case SDL_EVENT_QUIT:
        return SDL_APP_FAILURE;
        break;
    case SDL_EVENT_WINDOW_RESIZED:
        helix.pixelGrid->handleResize(event);
        break;
    case SDL_EVENT_MOUSE_MOTION:
        // TODO: Move "selection square/pixel" to mouse position
        // helix.selectionCursor->handleMouseEvent(event);
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        helix.pixelGrid->handleMouseEvent(event, helix.brush.fillColor);
        break;
    case SDL_EVENT_USER:
        helix.pixelGrid->handleZoom(event);
    }

    HLX::GUI::handleEvent(event);
    return SDL_APP_CONTINUE;
};

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    HLX::HelixState &helix = *static_cast<HLX::HelixState *>(appstate);
    SDL_DestroyTexture(helix.background);
    SDL_DestroyRenderer(helix.renderer);
    SDL_DestroyWindow(helix.window);
    SDL_Quit();
};
