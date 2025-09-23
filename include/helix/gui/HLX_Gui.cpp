#include "HLX_Gui.h"
#include "HLX_GuiElement.h"
#include "elements/HLX_Toolbar.h"
#include "imgui.h"

namespace HLX {
Gui::Gui(SDL_Window *window, SDL_Renderer *renderer) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad;            // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle &style = ImGui::GetStyle();
    style = ImGui::GetStyle();
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    mGuiElements.emplace_back(new Toolbar());
};

Gui::~Gui() {
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
};

void Gui::newFrame() {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
};

void Gui::renderElements() {
    for (GuiElement *element : mGuiElements) {
        element->render();
    };
};

void Gui::renderFrame(SDL_Renderer *renderer) {
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
};

void Gui::handleEvent(SDL_Event *event) { ImGui_ImplSDL3_ProcessEvent(event); };
}; // namespace HLX
