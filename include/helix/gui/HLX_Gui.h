#pragma once

#include "../HLX_pch.h"
#include "HLX_Palette.h"
#include "HLX_Toolbox.h"

namespace HLX {
namespace GUI {

static bool isHelixGuiInitialized = false;

void init(SDL_Renderer *renderer, SDL_Window *window);
void newFrame();
void renderFrame(SDL_Renderer *renderer);
void shutdown();

void handleEvent(SDL_Event *event);

void renderPalette(Palette &palette);
void renderToolbox(Toolbox &toolbox);
void renderToolbar();

}; // namespace GUI
}; // namespace HLX
