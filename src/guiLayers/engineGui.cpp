#include "nmpch.hpp"
#include "core.hpp"

#include "guiLayers/engineGui.hpp"
#include "application.hpp"
#include "platform/graphicsApi.hpp"

#include "imgui.h"

namespace nimbus
{

// helpers have to be here to avoid circular dependency if they are in
// class declaration due to needing application, and application includes
// this class...
static Application* sp_appRef    = nullptr;
static Window*      sp_appWinRef = nullptr;

EngineGui::EngineGui()
    : Layer(Layer::Type::OVERLAY, "engineGUI")
{
    sp_appRef    = &Application::get();
    sp_appWinRef = &sp_appRef->getWindow();

    m_frameTimes_ms.reserve(k_frameHistoryLength);
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


    // this is kinda inefficient, could draw over instead of
    // scroll?
    if(m_frameTimes_ms.size() == m_frameTimes_ms.capacity())
    {
        m_frameTimes_ms.erase(m_frameTimes_ms.begin());
    }
    m_frameTimes_ms.push_back(sp_appWinRef->m_tFrame_s * 1000.0);

    ImGui::PlotLines("Frame Times",
                     m_frameTimes_ms.data(),
                     m_frameTimes_ms.size(),
                     0,
                     nullptr,
                     0.0f,
                     20.0f,
                     ImVec2(0, 0),
                     sizeof(float));

    ImGui::Text("Draw Parameters");

    bool newWireFrameMode = GraphicsApi::getWireframe();
    ImGui::Checkbox("Wireframe Mode", &newWireFrameMode);
    GraphicsApi::setWireframe(newWireFrameMode);

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

    // ImGui::ShowDemoWindow();

}

}  // namespace nimbus