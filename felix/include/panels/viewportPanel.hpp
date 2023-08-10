#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/core/application.hpp"
#include "nimbus/core/window.hpp"
#include "nimbus/core/utility.hpp"
#include "nimbus/renderer/frameBuffer.hpp"

#include "glm.hpp"
#include "ImGuizmo.h"

namespace nimbus
{

class ViewportPanel
{
   public:
    glm::vec2 m_viewportSize = {800, 600};
    glm::vec2 m_viewportRegion[2];

    bool      m_viewportFocused;
    bool      m_viewportHovered;

    bool m_wireFrame = false;

    ViewportPanel(Camera* p_editCamera)
    {
        mp_appRef    = &Application::s_get();
        mp_appWinRef = &mp_appRef->getWindow();

        mp_editCamera   = p_editCamera;

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

    void onDraw(ref<FrameBuffer>& p_screenBuffer,
                bool              orthographic,
                Camera::Bounds&   bounds,
                Entity            selectedEntity)
    {
        ImGui::SetNextWindowSize({m_viewportSize.x, m_viewportSize.y},
                                 ImGuiCond_FirstUseEver);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("Viewport");

        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset    = ImGui::GetWindowPos();

        m_viewportRegion[0] = {viewportMinRegion.x + viewportOffset.x,
                               viewportMinRegion.y + viewportOffset.y};
        m_viewportRegion[1] = {viewportMaxRegion.x + viewportOffset.x,
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

        if (textureID != 0)
        {
            ImGui::Image(reinterpret_cast<void*>(textureID),
                         ImVec2{m_viewportSize.x, m_viewportSize.y},
                         ImVec2{0, 1},
                         ImVec2{1, 0});
        }

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
        if (m_viewportHovered && orthographic)
        {
            ImVec2 mousePos = ImGui::GetMousePos();

            glm::vec2 mousePosInViewportPix
                = {mousePos.x - windowPos.x,
                   mousePos.y - windowPos.y - titleBarHeight};

            glm::vec2 mousePosInViewport = util::mapPixToScreen(
                {mousePosInViewportPix.x, mousePosInViewportPix.y},
                bounds.topLeft.x,
                bounds.topRight.x,
                bounds.topLeft.y,
                bounds.bottomLeft.y,
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

        if (selectedEntity)
        {
            if (selectedEntity.hasComponent<TransformCmp>())
            {
                ImGuizmo::SetOrthographic(orthographic);
                ImGuizmo::SetDrawlist();

                ImGuizmo::SetRect(
                    m_viewportRegion[0].x,
                    m_viewportRegion[0].y,
                    m_viewportRegion[1].x - m_viewportRegion[0].x,
                    m_viewportRegion[1].y - m_viewportRegion[0].y);

                const glm::mat4& cameraView = mp_editCamera->getView();
                const glm::mat4& cameraProjection
                    = mp_editCamera->getProjection();

                auto&     tc = selectedEntity.getComponent<TransformCmp>();
                glm::mat4 transform = tc.getTransform();

                ImGuizmo::Manipulate(glm::value_ptr(cameraView),
                                     glm::value_ptr(cameraProjection),
                                     ImGuizmo::OPERATION::SCALE,
                                     ImGuizmo::LOCAL,
                                     glm::value_ptr(transform),
                                     nullptr,
                                     nullptr);

                if (ImGuizmo::IsUsing())
                {
                    tc.setTransform(transform);
                }
            }
        }

        ImGui::PopStyleVar();

        ImGui::End();  // viewport
    }

   private:
    Application* mp_appRef;
    Window*      mp_appWinRef;

    bool m_wasResized;

    Camera* mp_editCamera;
};

}  // namespace nimbus