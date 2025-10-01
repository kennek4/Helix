#pragma once

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
#include <imgui.h>

namespace HLX {
namespace GUI {

static bool isHelixGuiInitialized = false;

void init(SDL_Renderer *renderer, SDL_Window *window);
void newFrame();
void renderFrame(SDL_Renderer *renderer);
void shutdown();

void handleEvent(SDL_Event *event);

void renderPalette(ImVec4 &rawColor);
void renderToolbar();

}; // namespace GUI
}; // namespace HLX
