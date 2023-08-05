#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/core/application.hpp"
#include "nimbus/core/window.hpp"
#include "nimbus/core/utility.hpp"
#include "nimbus/renderer/frameBuffer.hpp"

#include "glm.hpp"

namespace nimbus
{

class ViewportPanel
{
   public:
    glm::vec2 m_viewportSize = {800, 600};
    bool      m_viewportFocused;
    bool      m_viewportHovered;

    bool m_wireFrame = false;

    ViewportPanel()
    {
        mp_appRef    = &Application::s_get();
        mp_appWinRef = &mp_appRef->getWindow();
    }
    ~ViewportPanel()
    {
    }

    bool wasResized()
    {
        if (m_wasResized)
        {
            m_wasResized = false;
            return true;
        }
        else
        {
            return false;
        }
    }

    void onDraw(ref<FrameBuffer>& p_screenBuffer, Camera::Bounds* p_bounds)
    {
        ImGui::SetNextWindowSize({m_viewportSize.x, m_viewportSize.y},
                                 ImGuiCond_FirstUseEver);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("Viewport");

        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset    = ImGui::GetWindowPos();

        glm::vec2 m_viewPortRegion[2];

        m_viewPortRegion[0] = {viewportMinRegion.x + viewportOffset.x,
                               viewportMinRegion.y + viewportOffset.y};
        m_viewPortRegion[1] = {viewportMaxRegion.x + viewportOffset.x,
                               viewportMaxRegion.y + viewportOffset.y};

        m_viewportFocused = ImGui::IsWindowFocused();
        m_viewportHovered = ImGui::IsWindowHovered();

        mp_appRef->guiSubsystemCaptureEvents(!m_viewportHovered);

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

        if (viewportPanelSize.x != m_viewportSize.x
            || viewportPanelSize.y != m_viewportSize.y)
        {
            // we need to resize
            m_viewportSize = {viewportPanelSize.x, viewportPanelSize.y};
            m_wasResized = true;
        }

        uint64_t textureID = p_screenBuffer->getTextureId();

        ImGui::Image(reinterpret_cast<void*>(textureID),
                     ImVec2{m_viewportSize.x, m_viewportSize.y},
                     ImVec2{0, 1},
                     ImVec2{1, 0});

        float titleBarHeight
            = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;

        ///////////////////////////
        // Draw the Viewport Size
        ///////////////////////////
        // Get the top-left corner of
        // the current ImGui window
        ImVec2 windowPos = ImGui::GetWindowPos();

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        char dimString[32];

        snprintf(dimString,
                 sizeof(dimString),
                 "%i x %i (%.03f)",
                 (int)m_viewportSize.x,
                 (int)m_viewportSize.y,
                 m_viewportSize.x / m_viewportSize.y);

        ImVec2 dimLoc = {windowPos.x + m_viewportSize.x - 105.0f,
                         windowPos.y + m_viewportSize.y - 17.0f};

        // Draw the text on the draw list
        drawList->AddText(dimLoc, IM_COL32(255, 255, 255, 255), dimString);

        ///////////////////////////
        // Draw the cursor pos
        ///////////////////////////
        if (m_viewportHovered && p_bounds != nullptr)
        {
            ImVec2 mousePos = ImGui::GetMousePos();

            glm::vec2 mousePosInViewportPix
                = {mousePos.x - windowPos.x,
                   mousePos.y - windowPos.y - titleBarHeight};

            glm::vec2 mousePosInViewport = util::mapPixToScreen(
                {mousePosInViewportPix.x, mousePosInViewportPix.y},
                p_bounds->topLeft.x,
                p_bounds->topRight.x,
                p_bounds->topLeft.y,
                p_bounds->bottomLeft.y,
                m_viewportSize.x,
                m_viewportSize.y);

            // draw mouse position over image
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            char posString[32];

            snprintf(posString,
                     sizeof(posString),
                     "X:%+.04f, Y:%+.04f",
                     mousePosInViewport.x,
                     mousePosInViewport.y);

            ImVec2 posLoc
                = {windowPos.x + 5.0f, windowPos.y + m_viewportSize.y - 17.0f};

            // Draw the text on the draw list
            drawList->AddText(posLoc, IM_COL32(255, 255, 255, 255), posString);
        }

        ImGui::PopStyleVar();

        ImGui::End();  // viewport
    }

   private:
    Application* mp_appRef;
    Window*      mp_appWinRef;

    bool         m_wasResized;

};

}  // namespace nimbus