#pragma once

#include "HLX_GuiElement.h"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
#include <imgui.h>
#include <vector>

namespace HLX {
class Gui {
  public:
    Gui(SDL_Window *window, SDL_Renderer *renderer);
    ~Gui();

    void newFrame();
    void renderFrame(SDL_Renderer *renderer);

    void renderElements();

    void displayToolbar();
    void displayPalette();

    void render(SDL_Renderer *renderer);
    void handleEvent(SDL_Event *event);

  private:
    std::vector<HLX::GuiElement *> mGuiElements;
};
}; // namespace HLX
