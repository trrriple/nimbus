#pragma once

#include "nimbus/core/common.hpp"

#include "imgui.h"

namespace nimbus::widgets
{

bool stateButton(const char* label, bool enabled, const ImVec2& size = ImVec2(0.0f, 0.0f))
{
    bool clicked = false;
    if (enabled)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);

        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
    }

    if (ImGui::Button(label, size))
    {
        clicked = true;
    }

    if (enabled)
        ImGui::PopStyleColor(2);

    return clicked;
}



};  // namespace nimbus::widgets