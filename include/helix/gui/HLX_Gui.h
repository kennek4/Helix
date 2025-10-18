#pragma once

#include "../HLX_pch.h"
#include <HLX_Constants.h>
#include <HLX_EventSystem.h>
#include <HLX_Toolbox.h>
#include <HLX_Types.h>

namespace HLX {
namespace GUI {

void init(SDL_Renderer *renderer, SDL_Window *window);
void shutdown();
void handleEvent(SDL_Event *event);

void createFrame();
void renderFrame(SDL_Renderer *renderer);
void renderElements(GuiProps &props, Toolbox &toolbox, const Grid &grid);

void renderToolbox(GuiProps &props, Toolbox &toolbox);
void renderToolbar(GuiProps &props);

void showSaveScreen(const Grid &grid);
void showKeybindMenu();
void showCreditsScreen();
void showExitConfirmation();

}; // namespace GUI
}; // namespace HLX
