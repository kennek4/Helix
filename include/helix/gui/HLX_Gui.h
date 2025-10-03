#pragma once

#include "../HLX_pch.h"

namespace HLX {
namespace GUI {

static bool isHelixGuiInitialized = false;

void init(SDL_Renderer *renderer, SDL_Window *window);
void newFrame();
void renderFrame(SDL_Renderer *renderer);
void shutdown();

void handleEvent(SDL_Event *event);

void renderPalette(ImVec4 &rawColor, SDL_FColor &color);
void renderToolbar();

}; // namespace GUI
}; // namespace HLX
