#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/core/application.hpp"
#include "nimbus/core/window.hpp"

namespace nimbus
{

class RenderStatsPanel
{
   public:
    bool m_wireFrame = false;

    RenderStatsPanel()
    {
        mp_appRef    = &Application::s_get();
        mp_appWinRef = &mp_appRef->getWindow();

        m_frameTimes_ms.reserve(k_frameHistoryLength);
    }
    ~RenderStatsPanel()
    {
    }

    void onDraw(float deltaTime)
    {
        NM_UNUSED(deltaTime);
        char buf[128];
        snprintf(buf,
                 128,
                 "%.02f ms/frame (%.02f FPS)###RenderStatus",
                 mp_appWinRef->m_tFrame_s * 1000.0f,
                 mp_appWinRef->m_fps);

        ImGui::Begin(buf, 0);

        bool newVsyncMode = mp_appWinRef->getVSync();
        if (ImGui::Checkbox("Vertical Sync", &newVsyncMode))
        {
            mp_appWinRef->setVSync(newVsyncMode);
        }

        ImGui::SameLine();

        ImGui::Checkbox("Wireframe Mode", &m_wireFrame);

        if (ImGui::Checkbox("Depth Test", &m_depthTest))
        {
            GraphicsApi::setDepthTest(m_depthTest);
        }

        // TODO Temporary
        static bool showDemoWindow = false;
        ImGui::Checkbox("Show ImGuiDemo Window", &showDemoWindow);
        if (showDemoWindow)
        {
            ImGui::ShowDemoWindow();
        }

        // this is kinda inefficient, could draw over instead of
        // scroll?
        if (m_frameTimes_ms.size() == m_frameTimes_ms.capacity())
        {
            m_frameTimes_ms.erase(m_frameTimes_ms.begin());
        }
        m_frameTimes_ms.push_back(mp_appWinRef->m_tFrame_s * 1000.0);

     

        ImGui::Separator();

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("Stats", tab_bar_flags))
        {
            if (ImGui::BeginTabItem("Performance"))
            {
                ImGui::PlotLines("Frame Times",
                                 m_frameTimes_ms.data(),
                                 m_frameTimes_ms.size(),
                                 0,
                                 nullptr,
                                 0.0f,
                                 20.0f,
                                 ImVec2(0, 0),
                                 sizeof(float));

                
                ImGui::PushItemWidth(70.0f);
                const auto swBank = mp_appRef->getSwBank().getBank();
                for (const auto& pair : swBank)
                {
                    ImGui::LabelText(pair.first,
                                     "%05.2f ms",
                                     pair.second->getLastSavedSplit() * 1000.0);
                }
                ImGui::PopItemWidth();


                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Calls"))
            {
                Renderer2D::Stats stats = Renderer2D::s_getStats();

                ImGui::PushItemWidth(60.0f);
                ImGui::LabelText("Draw Calls", "%i", stats.drawCalls);
                ImGui::LabelText("Quads", "%i", stats.quads);
                ImGui::LabelText("Characters", "%i", stats.characters);

                ImGui::LabelText("Total Vertices", "%i", stats.totalVertices);
                ImGui::LabelText(
                    "Quad Vertices Available", "%i", stats.quadVertsAvail);
                ImGui::LabelText(
                    "Text Vertices Available", "%i", stats.textVertsAvail);

                ImGui::PopItemWidth();

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Layers"))
            {
                if (ImGui::BeginTable("Layer Order", 2))
                {
                    ImGui::TableSetupColumn("#");
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableHeadersRow();

                    auto layerNames = mp_appRef->getLayerDeck().getLayerNames();
                    for (uint32_t i = 0; i < layerNames.size(); i++)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Text("%d", i);
                        ImGui::TableNextColumn();
                        ImGui::Text("%s", layerNames[i].c_str());
                    }

                    ImGui::EndTable();
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::End();
    }

   private:
    Application* mp_appRef;
    Window*      mp_appWinRef;
    bool         m_depthTest = false;

    inline static const uint32_t k_frameHistoryLength = 60 * 2 + 1;
    std::vector<float>           m_frameTimes_ms;
};

}  // namespace nimbus