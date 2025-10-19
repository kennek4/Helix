#include "HLX_Gui.h"
#include "Helix.h"
#include <SDL3/SDL_mouse.h>

#define SDL_MAIN_USE_CALLBACKS
#include "SDL3/SDL_main.h"

static bool isMouseDown = false;
static int lastMouseButton;

static bool isFullscreen = true;

constexpr SDL_InitFlags initFlags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    *appstate = new HLX::HelixState;
    HLX::HelixState &helix = *static_cast<HLX::HelixState *>(*appstate);

    helix.window = new HLX::Window(&helix.sdlProps, &helix.windowProps);
    if (!helix.window->init()) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                        "Failed to initialize Helix, %s", SDL_GetError());
        return SDL_APP_FAILURE;
    };

    // TODO: GUI error handling here
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Initializing HLX::GUI");
    HLX::GUI::init(helix.sdlProps.renderer, helix.sdlProps.window);

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Initializing HLX::PixelGrid");
    helix.pixelGrid = new HLX::PixelGrid(&helix.sdlProps, 32, 32);
    helix.pixelGrid->init();

    helix.renderer = new HLX::Renderer(helix.sdlProps.renderer);

    if (!helix.pixelGrid->init()) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                        "Failed to initialize PixelGrid, %s", SDL_GetError());
        return SDL_APP_FAILURE;
    };

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "Successfully initialized Helix!");
    return SDL_APP_CONTINUE;
};

// Main Application Loop
SDL_AppResult SDL_AppIterate(void *appstate) {
    HLX::HelixState &helix = *static_cast<HLX::HelixState *>(appstate);

    // Process Input
    // NOTE: This allows mouse painting / dragging
    if (isMouseDown) {
        helix.toolbox.useTool(lastMouseButton);
    };

    // NOTE: Clear screen
    helix.renderer->clearScreen();
    helix.renderer->createBackground(helix.pixelGrid->getBackgroundFRect(),
                                     25.0f / 256.0f);
    helix.renderer->createGrid(helix.pixelGrid->getGridData());

    HLX::GUI::createFrame();
    HLX::GUI::renderToolbox(helix.guiProps, helix.toolbox);
    HLX::GUI::renderToolbar(helix.guiProps);
    HLX::GUI::renderElements(helix.guiProps, helix.toolbox,
                             helix.pixelGrid->getGridData());
    HLX::GUI::renderFrame(helix.sdlProps.renderer);

    helix.renderer->render();
    return SDL_APP_CONTINUE; /* carry on with the program! */
};

// Event Handling
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    HLX::HelixState &helix = *static_cast<HLX::HelixState *>(appstate);
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_FAILURE;
    };

    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        lastMouseButton = event->button.button;
        isMouseDown = true;
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        isMouseDown = false;
    };

    HLX::EventSystem::getInstance().publishToTopic(event);
    HLX::GUI::handleEvent(event);
    return SDL_APP_CONTINUE;
};

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    HLX::HelixState &helix = *static_cast<HLX::HelixState *>(appstate);
    // SDL_DestroyTexture(helix.pixelGrid.);
    SDL_DestroyRenderer(helix.sdlProps.renderer);
    SDL_DestroyWindow(helix.sdlProps.window);
    SDL_Quit();
};
