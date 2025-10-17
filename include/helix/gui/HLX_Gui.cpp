#include "HLX_Gui.h"
#include "HLX_Constants.h"
#include "imgui.h"
#include <string_view>

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

void renderToolbox(Toolbox &toolbox) {
    constexpr ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize;

    static int selectedTool = 0;
    static bool needsUpdate = true;

    ImGui::Begin("Toolbox", NULL, windowFlags);
    ImGui::SeparatorText("Tools");

    for (int i = 0; i < Constants::EventCodes.size(); i++) {
        ImGui::PushID(i);

        const Sint32 &eventCode = Constants::EventCodes[i];
        const std::string_view &label =
            Constants::ToolEventCodeToNameMap[eventCode];

        if (ImGui::RadioButton(label.data(), &selectedTool, i)) {
            needsUpdate = true;
        };

        ImGui::PopID();
    };

    ImGui::SeparatorText("Brush Sizes");
    ImGui::RadioButton("1", toolbox.getToolSize(), 1);
    ImGui::SameLine();
    ImGui::RadioButton("2", toolbox.getToolSize(), 2);
    ImGui::SameLine();
    ImGui::RadioButton("3", toolbox.getToolSize(), 3);
    ImGui::SameLine();
    ImGui::RadioButton("4", toolbox.getToolSize(), 4);

    ImGui::SeparatorText("Brush Colors");
    ImGui::ColorEdit4("Primary Color", (float *)&toolbox.getToolColor()[0],
                      ImGuiColorEditFlags_NoInputs);

    ImGui::End();

    if (needsUpdate) {
        toolbox.setTool(selectedTool);
        needsUpdate = false;
    }
};

void renderToolbar() {
    constexpr ImGuiWindowFlags winFlags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;
    ;
    ;

    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
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
            if (ImGui::MenuItem("Save Image")) {
            };
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
