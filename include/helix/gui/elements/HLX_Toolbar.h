#pragma once
#include "HLX_GuiElement.h"
#include "imgui.h"

namespace HLX {
class Toolbar : public GuiElement {
    virtual void render() override {
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
        }
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Options")) {
                ImGui::Separator();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("File")) {
                ImGui::Separator();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                ImGui::Separator();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help")) {
                ImGui::Separator();

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();
    };
};
}; // namespace HLX
