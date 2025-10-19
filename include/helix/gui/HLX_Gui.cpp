#include "HLX_Gui.h"
#include "HLX_Constants.h"
#include "HLX_EventSystem.h"
#include "HLX_Types.h"
#include "ImGuiFileDialog.h"
#include "image/HLX_Image.h"
#include "imgui.h"
#include <SDL3/SDL_log.h>

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

void renderElements(GuiProps &props, Toolbox &toolbox, const Grid &grid) {
    if (props.isSaveScreenActive) {
        showSaveScreen(grid);
    };

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

    ImGuiIO &io = ImGui::GetIO();
    const float &center = ImGui::GetMainViewport()->GetCenter().y;

    ImGui::SetNextWindowPos(ImVec2(0.0f, center), ImGuiCond_Always,
                            ImVec2(0.0f, 0.5f));

    ImGui::Begin("Toolbox", NULL, windowFlags);
    ImGui::SeparatorText("Tools");
    ImGui::NewLine();

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
    ImGui::SeparatorText("Colors");
    ImGui::NewLine();
    constexpr ImGuiColorEditFlags editFlags = ImGuiColorEditFlags_NoInputs |
                                              ImGuiColorEditFlags_NoLabel |
                                              ImGuiColorEditFlags_NoSidePreview;
    ImGui::Text("Primary");
    ImGui::SameLine();
    ImGui::ColorEdit4("Primary", (float *)&toolbox.getPrimaryColor()[0],
                      editFlags);

    ImGui::Text("Secondary");
    ImGui::SameLine();
    ImGui::ColorEdit4("Secondary", (float *)&toolbox.getSecondaryColor()[0],
                      editFlags);

    ImGui::NewLine();
    ImGui::SeparatorText("Tool Props.");
    ImGui::NewLine();
    ImGui::SliderInt("Tool Size", toolbox.getToolSize(), 1, 4);

    ImGui::NewLine();
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
    static bool isActive{true};
    static int selectedItem = 0;
    static const char *colorSchemePreview = Constants::ColorSchemes[0].data();

    if (ImGui::BeginMenu("View")) {
        if (ImGui::BeginCombo("Color Schemes", colorSchemePreview,
                              ImGuiComboFlags_HeightSmall |
                                  ImGuiComboFlags_NoPreview)) {
            for (int i = 0; i < Constants::ColorSchemes.size(); i++) {
                const bool isSelected = (selectedItem == i);
                if (ImGui::Selectable(Constants::ColorSchemes[i].data(),
                                      isSelected))
                    selectedItem = i;

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                };
            };

            ImGui::EndCombo();
        };

        switch (selectedItem) {
        case 0:
            ImGui::StyleColorsDark();
            break;
        case 1:
            ImGui::StyleColorsLight();
            break;
        case 2:
            ImGui::StyleColorsClassic();
            break;
        }

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

void showSaveScreen(const Grid &grid) {
    ImGuiIO &io = ImGui::GetIO();
    const float &winWidth = io.DisplaySize.x;
    const float &winHeight = io.DisplaySize.y;
    const ImVec2 &center = ImGui::GetMainViewport()->GetCenter();

    ImGui::SetNextWindowSize(ImVec2(winWidth * 0.5f, winHeight * 0.5f));
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    if (ImGuiFileDialog::Instance()->Display("SaveFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string fileName =
                ImGuiFileDialog::Instance()->GetCurrentFileName();
            std::string fileNamePath =
                ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath =
                ImGuiFileDialog::Instance()->GetCurrentPath();

            if (!HLX::Image::validFileName(fileName)) {
                SDL_Log("The file name path,  %s , given is invalid.",
                        fileName.c_str());

            } else {
                SDL_Log("Saving file %s at %s", fileName.c_str(),
                        filePath.c_str());
                HLX::Image::savePNG(grid.widthInPixels, grid.heightInPixels,
                                    grid.colors, fileNamePath.c_str());
            };

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
