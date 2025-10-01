#include "HLX_Gui.h"
#include "Helix.h"
#include "imgui.h"
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
    HLX::HelixState &state = *static_cast<HLX::HelixState *>(*appstate);

    SDL_Log("Creating new Window/Renderer...");
    if (!SDL_CreateWindowAndRenderer("Helix Sprite Maker", 1600, 900, winFlags,
                                     &state.window, &state.renderer)) {
        SDL_Log("[FAIL] SDL failed to create Window/Renderer: %s",
                SDL_GetError());
        return SDL_APP_FAILURE;
    };

    SDL_SetWindowFullscreen(state.window, true);

    HLX::GUI::init(state.renderer, state.window);

    SDL_GetWindowSize(state.window, &state.windowWidth, &state.windowHeight);
    SDL_Log("Window W: %d, Window Height :%d", state.windowWidth,
            state.windowHeight);

    // NOTE: Setup background image
    std::string bgPath = SDL_GetBasePath();
    bgPath += "checkerboard.bmp";

    SDL_Surface *bgSurface = SDL_LoadBMP(bgPath.c_str());
    if (bgSurface == nullptr) {
        SDL_Log("Failed to load background: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    };

    state.background = SDL_CreateTextureFromSurface(state.renderer, bgSurface);
    SDL_SetTextureBlendMode(state.background, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaModFloat(state.background, 0.1f);
    SDL_DestroySurface(bgSurface);

    state.backgroundTilingScale =
        (state.background->w / (float)state.windowWidth) / 4;

    HLX::PixelGridProperties props;
    props.windowWidth = &state.windowWidth;
    props.windowHeight = &state.windowHeight;
    props.gridWidth = 32;
    props.gridHeight = 32;

    // NOTE: 25 is the length of a pixel side, mid point x/y - 25 accounts for
    // how SDL_Rects start drawing at the top left corner
    // props.gridMidPoint = SDL_Point{(w / 2) - 25, (h / 2) - 25};
    state.pixelGrid = new HLX::PixelGrid(props, state.renderer);

    return SDL_APP_CONTINUE;
};

// Main Application Loop
SDL_AppResult SDL_AppIterate(void *appstate) {
    HLX::HelixState &state = *static_cast<HLX::HelixState *>(appstate);

    HLX::changeBrushColor(state.rawColor, state.brushColor);

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
    SDL_SetRenderDrawColor(state.renderer, 255, 255, 255,
                           SDL_ALPHA_OPAQUE); /* black, full alpha */
    SDL_RenderClear(state.renderer);          /* start with a blank canvas. */
    SDL_RenderTextureTiled(state.renderer, state.background, NULL,
                           state.backgroundTilingScale, NULL);

    HLX::GUI::newFrame();
    HLX::GUI::renderToolbar();
    HLX::GUI::renderPalette(state.rawColor);

    state.pixelGrid->render();

    HLX::GUI::renderFrame(state.renderer);
    SDL_RenderPresent(state.renderer); /* put it all on the screen! */
    return SDL_APP_CONTINUE;           /* carry on with the program! */
};

// Event Handling
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    HLX::HelixState &state = *static_cast<HLX::HelixState *>(appstate);
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
        state.pixelGrid->handleResize(event);
        break;
    case SDL_EVENT_MOUSE_MOTION:
        // TODO: Move "selection square/pixel" to mouse position
        // state.selectionCursor->handleMouseEvent(event);
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        state.pixelGrid->handleMouseEvent(event, state.brushColor);
        break;
    case SDL_EVENT_USER:
        state.pixelGrid->handleZoom(event);
    }

    HLX::GUI::handleEvent(event);
    return SDL_APP_CONTINUE;
};

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    HLX::HelixState &state = *static_cast<HLX::HelixState *>(appstate);
    SDL_DestroyTexture(state.background);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
};
