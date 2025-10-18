#include "HLX_Gui.h"
#include "HLX_Constants.h"
#include "HLX_EventSystem.h"
#include "HLX_Types.h"
#include "ImGuiFileDialog.h"
#include "imgui.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_stdinc.h>
#include <string>

namespace HLX {
namespace GUI {

void init(SDL_Renderer *renderer, SDL_Window *window) {
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
};

void shutdown() {
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
};

void handleEvent(SDL_Event *event) { ImGui_ImplSDL3_ProcessEvent(event); };

void createFrame() {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
};

void renderFrame(SDL_Renderer *renderer) {
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
};

void renderElements(GuiProps &props) {
    static SDL_Event event;
    if (props.isSaveScreenActive) {
        showSaveScreen(&props);
    }
    if (props.isKeybindMenuActive)
        showKeybindMenu();
    if (props.isCreditsScreenActive)
        showCreditsScreen();
};

void renderToolbox(GuiProps &props, Toolbox &toolbox) {
    constexpr ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize;

    static int selectedTool = 0;
    static bool needsUpdate = true;

    ImGui::Begin("Toolbox", NULL, windowFlags);
    ImGui::SeparatorText("Tools");

    for (int i = 0; i < Constants::ToolEventCodes.size(); i++) {
        ImGui::PushID(i);

        const Sint32 &eventCode = Constants::ToolEventCodes[i];
        const std::string_view &label =
            Constants::ToolEventCodeToNameMap[eventCode];

        if (ImGui::RadioButton(label.data(), &selectedTool, i)) {
            needsUpdate = true;
        };

        ImGui::PopID();
    };

    ImGui::NewLine();
    ImGui::NewLine();

    ImGui::SeparatorText("Brush Sizes");
    ImGui::RadioButton("1", toolbox.getToolSize(), 1);
    ImGui::SameLine();
    ImGui::RadioButton("2", toolbox.getToolSize(), 2);
    ImGui::SameLine();
    ImGui::RadioButton("3", toolbox.getToolSize(), 3);
    ImGui::SameLine();
    ImGui::RadioButton("4", toolbox.getToolSize(), 4);

    ImGui::NewLine();
    ImGui::NewLine();

    ImGui::SeparatorText("Colors");
    ImGui::ColorEdit4("Primary", (float *)&toolbox.getToolColor()[0],
                      ImGuiColorEditFlags_NoInputs);

    // ImGui::ColorEdit4("Secondary", NULL, ImGuiColorEditFlags_NoInputs);
    ImGui::End();

    if (needsUpdate) {
        toolbox.setTool(selectedTool);
        needsUpdate = false;
    }
};

void renderToolbar(GuiProps &props) {
    constexpr ImGuiWindowFlags winFlags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;

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

    ImGui::BeginMenuBar();
    if (ImGui::BeginMenu("File")) {
        if (ImGui::Button("Save Image As")) {
            static SDL_Event event;
            SDL_zero(event);

            event.type = Constants::HelixEvent;
            event.user.code = Constants::EventGUIHasPriority;

            props.isSaveScreenActive = true;
            IGFD::FileDialogConfig config;
            config.path = ".";
            config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;
            ImGuiFileDialog::Instance()->OpenDialog(
                "SaveFileDlgKey", "Save File As", ".png", config);

            EventSystem::getInstance().publishToTopic(&event);
        } else {
            props.isSaveScreenActive = false;
        };

        ImGui::EndMenu();
    }

    ImGui::Separator();
    if (ImGui::BeginMenu("View")) {
        ImGui::Text("Nothing here yet... :(");
        ImGui::EndMenu();
    }

    ImGui::Separator();
    if (ImGui::BeginMenu("Help")) {
        ImGui::MenuItem("Keybinds", "k", &props.isKeybindMenuActive);
        ImGui::MenuItem("Credits", "o", &props.isCreditsScreenActive);
        ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
    ImGui::End();
};

void showSaveScreen(GuiProps *props) {
    static std::string fileName{};
    static SDL_Event event;

    ImGuiIO &io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
    if (ImGuiFileDialog::Instance()->Display("SaveFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName =
                ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath =
                ImGuiFileDialog::Instance()->GetCurrentPath();

            SDL_Log("Saving file %s at %s", filePathName.c_str(),
                    filePath.c_str());

            static SDL_Event event;
            event.type = Constants::HelixEvent;
            event.user.code = Constants::EventGUINoPriority;
            EventSystem::getInstance().publishToTopic(&event);
        };
        ImGuiFileDialog::Instance()->Close();
    };
};

void showKeybindMenu() {

};

void showCreditsScreen() {

};

}; // namespace GUI
}; // namespace HLX
