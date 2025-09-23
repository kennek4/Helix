
#include "HLX_Gui.h"
#include "HLX_PixelGrid.h"
#include "backends/imgui_impl_sdl3.h"
#include "imgui.h"
#include <HLX_Application.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>

#define SDL_MAIN_USE_CALLBACKS
#include "SDL3/SDL_main.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

static SDL_Window *sdlWindow;
static SDL_Renderer *sdlRenderer;

static HLX::PixelGrid *myPixelGrid;
static HLX::Gui *myGui;

static bool showDemoWin = true;
static float zoom = 1.0f;
static int bgRed = 255;
static int bgBlue = 255;
static int bgGreen = 255;

static bool isMouseDown = false;

constexpr SDL_InitFlags initFlags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;
constexpr SDL_WindowFlags winFlags = SDL_WINDOW_HIGH_PIXEL_DENSITY |
                                     SDL_WINDOW_HIGH_PIXEL_DENSITY |
                                     SDL_WINDOW_INPUT_FOCUS;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    SDL_Log("Initializing Helix...");

    *appstate = new HLX::Application{};
    HLX::Application &app = *static_cast<HLX::Application *>(*appstate);

    SDL_Log("Creating new Window/Renderer...");
    if (!SDL_CreateWindowAndRenderer("Helix Sprite Maker", 1600, 900, winFlags,
                                     &sdlWindow, &sdlRenderer)) {
        SDL_Log("[FAIL] SDL failed to create Window/Renderer: %s",
                SDL_GetError());
        return SDL_APP_FAILURE;
    };

    myGui = new HLX::Gui(sdlWindow, sdlRenderer);

    HLX::PixelGridProperties props;
    props.gridWidth = 32;
    props.gridHeight = 32;
    props.gridStartPoint = SDL_Point{100, 100};
    myPixelGrid = new HLX::PixelGrid(props, sdlRenderer);
    return SDL_APP_CONTINUE;
};

// Main Application Loop
SDL_AppResult SDL_AppIterate(void *appstate) {
    HLX::Application &app = *static_cast<HLX::Application *>(appstate);

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

    myGui->newFrame();

    ImGui::Begin("Background Colour");
    ImGui::SliderInt("Red", &bgRed, 0, 255);
    ImGui::SliderInt("Green", &bgGreen, 0, 255);
    ImGui::SliderInt("Blue", &bgBlue, 0, 255);
    ImGui::End();

    SDL_SetRenderDrawColor(sdlRenderer, bgRed, bgGreen, bgBlue,
                           SDL_ALPHA_OPAQUE); /* black, full alpha */
    SDL_RenderClear(sdlRenderer);             /* start with a blank canvas. */

    myPixelGrid->render();
    myGui->renderElements();

    ImGui::Begin("Zoom Slider");
    ImGui::SliderFloat("Zoom", &zoom, 0.5f, 1.0f);
    ImGui::End();

    myGui->renderFrame(sdlRenderer);
    SDL_RenderPresent(sdlRenderer); /* put it all on the screen! */
    return SDL_APP_CONTINUE;        /* carry on with the program! */
};

// Event Handling
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    HLX::Application &app = *static_cast<HLX::Application *>(appstate);

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
        myPixelGrid->handleResize(event);
        break;

    case SDL_EVENT_MOUSE_MOTION:
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        myPixelGrid->handleMouseEvent(event);
        break;
    case SDL_EVENT_USER:
        myPixelGrid->handleZoom(event);
    }

    myGui->handleEvent(event);
    return SDL_APP_CONTINUE;
};

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    HLX::Application &app = *static_cast<HLX::Application *>(appstate);

    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
};
