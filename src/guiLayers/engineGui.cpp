#include "nmpch.hpp"
#include "core.hpp"

#include "guiLayers/engineGui.hpp"
#include "application.hpp"
#include "platform/rendererApi.hpp"

#include "imgui.h"

namespace nimbus
{

// helpers have to be here to avoid circular dependency if they are in
// class declaration due to needing application, and application includes
// this class...
static Application* sp_appRef    = nullptr;
static Window*      sp_appWinRef = nullptr;

EngineGui::EngineGui() : Layer(Layer::Type::OVERLAY, "engineGUI")
{
    sp_appRef    = &Application::get();
    sp_appWinRef = &sp_appRef->getWindow();
}

void EngineGui::onInsert()
{
}

void EngineGui::onRemove()
{
}


void EngineGui::onEvent(Event& event)
{
    UNUSED(event);
}

void EngineGui::onGuiUpdate()
{
    // todo handle enable/disable for this
    char buf[128];
    snprintf(buf,
             128,
             "%.02f ms/frame (%.02f FPS)###RenderStatus",
             sp_appRef->getFrametime() * 1000.0f,
             sp_appWinRef->m_fps);

    ImGui::Begin(buf,
                 0,
                 ImGuiWindowFlags_AlwaysAutoResize
                     | ImGuiWindowFlags_NoFocusOnAppearing);

    ImGui::Text("Draw Parameters");

    bool newWireFrameMode = RendererApi::getWireframe();
    ImGui::Checkbox("Wireframe Mode", &newWireFrameMode);
    RendererApi::setWireframe(newWireFrameMode);

    ImGui::SameLine();

    bool newVsyncMode = sp_appWinRef->getVSync();
    ImGui::Checkbox("Vertical Sync", &newVsyncMode);
    sp_appWinRef->setVSync(newVsyncMode);

    if (ImGui::CollapsingHeader("Layers"))
    {
        if (ImGui::BeginTable("Layer Order", 2))
        {
            ImGui::TableSetupColumn("#");
            ImGui::TableSetupColumn("Name");
            ImGui::TableHeadersRow();

            auto layerNames = sp_appRef->getLayerDeck().getLayerNames();
            for (uint32_t i = 0; i < layerNames.size(); i++)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%d", i);
                ImGui::TableNextColumn();
                ImGui::Text("%s", layerNames[i]->c_str());
            }

            ImGui::EndTable();
        }
    }

    ImGui::End();

#if 0
    ImGui::Begin("Camera Menu", 0, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Camera Pos (X: %.03f, Y: %.03f, Z: %.03f)",
                mp_camera->m_position[0],
                mp_camera->m_position[1],
                mp_camera->m_position[2]);

    ImGui::Text("Camera Attitude (Yaw: %.03f, Pitch: %.03f)",
                mp_camera->m_yaw,
                mp_camera->m_pitch);

    ImGui::SliderFloat("Speed", &mp_camera->m_speed, 1.0f, 100.0f);
    ImGui::SliderFloat("Sensitivity", &mp_camera->m_sensitivity, 0.01f, 0.5f);
    ImGui::End();
#endif

    // ImGui::ShowDemoWindow();

}

}  // namespace nimbus