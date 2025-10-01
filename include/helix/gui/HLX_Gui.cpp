#include "HLX_Gui.h"
#include "imgui.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

namespace HLX {
namespace GUI {

void init(SDL_Renderer *renderer, SDL_Window *window) {
    if (isHelixGuiInitialized) {
        // TODO: Quit program shouldn't happen
    };
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

    isHelixGuiInitialized = true;
};

void newFrame() {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
};

void renderFrame(SDL_Renderer *renderer) {
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
};

void handleEvent(SDL_Event *event) { ImGui_ImplSDL3_ProcessEvent(event); };

void shutdown() {
    if (!isHelixGuiInitialized) {
        // TODO: Return false, shouldn't happen
        return;
    };

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
};

// TODO: renderPalette should have a either a callback function as an argument
// OR give a pointer/ref to the brush color? mabybe potentially even just give
// renderPalette a reference to a Brush struct?

void renderPalette(ImVec4 &rawColor, SDL_FColor &color) {
    ImGui::Begin("ColorPicker");
    ImGui::ColorPicker4("HelixColorPicker", (float *)&rawColor);

    if (ImGui::IsItemEdited()) {
        color = {rawColor.x, rawColor.y, rawColor.z, rawColor.w};
    };

    ImGui::End();
};

void renderToolbar() {
    ImGuiWindowFlags winFlags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    winFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    winFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
                ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGui::Begin("Toolbar", NULL, winFlags);

    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f),
                         ImGuiDockNodeFlags_PassthruCentralNode |
                             ImGuiDockNodeFlags_NoDockingOverCentralNode);
    };

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Options")) {
            ImGui::MenuItem("Keybinds");
            ImGui::MenuItem("Window Settings");
            ImGui::EndMenu();
        }

        ImGui::Separator();

        if (ImGui::BeginMenu("File")) {

            ImGui::EndMenu();
        }

        ImGui::Separator();
        if (ImGui::BeginMenu("View")) {

            ImGui::EndMenu();
        }

        ImGui::Separator();
        if (ImGui::BeginMenu("Help")) {

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();
};

}; // namespace GUI
}; // namespace HLX
