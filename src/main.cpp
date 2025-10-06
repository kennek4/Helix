#include "HLX_Window.h"
#include "Helix.h"
#include "imgui.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_video.h>

#define SDL_MAIN_USE_CALLBACKS
#include "SDL3/SDL_main.h"

static bool isMouseDown = false;
static bool isFullscreen = true;

constexpr SDL_InitFlags initFlags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    *appstate = new HLX::HelixState;
    HLX::HelixState &helix = *static_cast<HLX::HelixState *>(*appstate);

    helix.window = new HLX::Window(helix.sdlProps, helix.windowProps);
    if (!helix.window->init()) {
        SDL_Log("%s", SDL_GetError());
        return SDL_APP_FAILURE;
    };

    // TODO: GUI error handling here
    SDL_Log("Initializing GUI...");
    HLX::GUI::init(helix.sdlProps.renderer, helix.sdlProps.window);

    // NOTE:
    // TODO: Grid WxH should be determined later on by a modal,
    // asking the dimensions of the sprite, file format(?), etc.

    SDL_Log("Initializing PixelGrid...");
    helix.pixelGridState.gridWidth = 32;
    helix.pixelGridState.gridHeight = 32;
    helix.pixelGrid =
        new HLX::PixelGrid(&helix.pixelGridState, helix.sdlProps,
                           helix.windowProps.width, helix.windowProps.height);

    if (!helix.pixelGrid->init()) {
        SDL_Log("%s", SDL_GetError());
        return SDL_APP_FAILURE;
    };

    SDL_Log("Helix Initialized!");
    return SDL_APP_CONTINUE;
};

// Main Application Loop
SDL_AppResult SDL_AppIterate(void *appstate) {
    HLX::HelixState &helix = *static_cast<HLX::HelixState *>(appstate);

    // Process Input
    // NOTE: This allows mouse painting / dragging
    if (isMouseDown) {
        SDL_Event hlxMouseUp;
        float x, y;

        SDL_zero(hlxMouseUp);
        hlxMouseUp.type = SDL_EVENT_USER;
        SDL_GetMouseState(&x, &y);

        hlxMouseUp.motion.x = x;
        hlxMouseUp.motion.y = y;
        hlxMouseUp.user.data1 = &helix.brush.fillColor;

        SDL_PushEvent(&hlxMouseUp);
    };

    // helixPixelGrid->handleMouseDrag(isMouseDown);

    // NOTE: Clear screen
    SDL_SetRenderDrawColor(helix.sdlProps.renderer, 255, 255, 255,
                           SDL_ALPHA_OPAQUE); /* black, full alpha */
    SDL_RenderClear(helix.sdlProps.renderer); /* start with a blank canvas. */

    HLX::GUI::newFrame();
    HLX::GUI::renderToolbar();
    HLX::GUI::renderPalette(helix.brush.rawColor, helix.brush.fillColor);

    ImGui::Begin("Save button");

    if (ImGui::Button("Save")) {
        SDL_Log("Saving img...");
        helix.pixelGrid->saveImage();
    };

    if (ImGui::Button("Resize Screen")) {
        SDL_Log("Force resizing screen");
        SDL_SetWindowFullscreen(helix.sdlProps.window, false);
        SDL_SetWindowSize(helix.sdlProps.window, 1280, 720);
    }

    if (ImGui::Button("Reset Canvas")) {
        helix.pixelGrid->reset();
    };

    ImGui::End();

    helix.pixelGrid->render();
    HLX::GUI::renderFrame(helix.sdlProps.renderer);
    SDL_RenderPresent(helix.sdlProps.renderer); /* put it all on the screen! */
    return SDL_APP_CONTINUE;                    /* carry on with the program! */
};

// Event Handling
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    HLX::HelixState &helix = *static_cast<HLX::HelixState *>(appstate);

    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        isMouseDown = true;
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        isMouseDown = false;
    };

    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_FAILURE;
    };

    HLX::EventSystem::getInstance().publishToTopic(event);
    HLX::GUI::handleEvent(event);
    return SDL_APP_CONTINUE;
};

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    HLX::HelixState &helix = *static_cast<HLX::HelixState *>(appstate);
    SDL_DestroyTexture(helix.background);
    SDL_DestroyRenderer(helix.sdlProps.renderer);
    SDL_DestroyWindow(helix.sdlProps.window);
    SDL_Quit();
};
