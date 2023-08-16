#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/core/application.hpp"
#include "nimbus/core/window.hpp"
#include "nimbus/core/utility.hpp"
#include "nimbus/renderer/framebuffer.hpp"
#include "panels/sceneControlPanel.hpp"

#include "glm.hpp"
#include "ImGuizmo.h"

namespace nimbus
{

class ViewportPanel
{
   public:
    typedef std::function<void(Entity)> EntitySelectedCallback_t;

    glm::vec2 m_viewportSize = {800, 600};
    glm::vec2 m_viewportRegion[2];
    bool      m_viewportFocused;
    bool      m_viewportHovered;
    bool      m_wireFrame = false;

    ViewportPanel(Camera* p_editCamera, ref<Scene> p_scene)
    {
        mp_appRef    = &Application::s_get();
        mp_appWinRef = &mp_appRef->getWindow();

        mp_editCamera   = p_editCamera;
        mp_sceneContext = p_scene;

        mp_pixelRequest = ref<Framebuffer::PixelReadRequest>::gen();
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

    void setEntitySelectedCallback(const EntitySelectedCallback_t& callback)
    {
        m_entitySelectedCallback = callback;
    }

    void onDraw(ref<Framebuffer>&            p_screenBuffer,
                bool                         orthographic,
                Camera::Bounds&              bounds,
                Entity                       selectedEntity,
                SceneControlPanel::ToolState toolState)
    {
        ImGui::SetNextWindowSize({m_viewportSize.x, m_viewportSize.y},
                                 ImGuiCond_FirstUseEver);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoTitleBar);

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
            m_wasResized   = true;
        }

        uint64_t textureID = p_screenBuffer->getTextureId();

        if (textureID != 0)
        {
            ImGui::Image(reinterpret_cast<void*>(textureID),
                         ImVec2{m_viewportSize.x, m_viewportSize.y},
                         ImVec2{0, 1},
                         ImVec2{1, 0});
        }

        ///////////////////////////
        // Draw the Viewport Size
        ///////////////////////////
        // Get the top-left corner of
        // the current ImGui window
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        char dimString[32];

        snprintf(dimString,
                 sizeof(dimString),
                 "%i x %i (%.03f)",
                 (int)m_viewportSize.x,
                 (int)m_viewportSize.y,
                 m_viewportSize.x / m_viewportSize.y);

        ImVec2 dimLoc
            = {m_viewportRegion[1].x - 115.0f, m_viewportRegion[1].y - 17.0f};

        // Draw the text on the draw list
        drawList->AddText(dimLoc, IM_COL32(255, 255, 255, 255), dimString);

        ///////////////////////////
        // Handle Cursor position
        ///////////////////////////
        if (m_viewportHovered)
        {
            ImVec2 mousePos = ImGui::GetMousePos();

            glm::vec2 mousePosInViewportPix
                = {mousePos.x - m_viewportRegion[0].x,
                   mousePos.y - m_viewportRegion[0].y};

            if (m_requestedPixelVal)
            {
                auto pv = mp_pixelRequest->getAndInvalidateValue();

                // because the rendering thread runs latent, the request
                // will be handled (typically) by the time this comes around
                // twice, i.e., the first time we check it will probably
                // not be valid.
                if (pv.valid)
                {
                    m_requestedPixelVal = false;

                    entt::entity id = static_cast<entt::entity>(
                        std::get<uint32_t>(pv.value));

                    if (mp_sceneContext->m_registry.valid((id)))
                    {
                        m_selectionContext = Entity(id, mp_sceneContext.raw());

                        m_entitySelectedCallback(m_selectionContext);
                    }
                }
            }

            if (ImGui::IsMouseClicked(0) && !ImGuizmo::IsOver())
            {
                uint32_t framebufPosInPixX = mousePosInViewportPix.x;
                uint32_t framebufPosInPixY
                    = m_viewportSize.y - mousePosInViewportPix.y;

                mp_pixelRequest->updateRequest(
                    1, framebufPosInPixX, framebufPosInPixY);

                p_screenBuffer->requestPixel(mp_pixelRequest);
                m_requestedPixelVal = true;
            }

            if (orthographic)
            {
                // if we're orthographic we can map where the curosr is in
                // the world easily
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

                ImVec2 posLoc = {m_viewportRegion[0].x + 5.0f,
                                 m_viewportRegion[1].y - 17.0f};

                // Draw the text on the draw list
                drawList->AddText(
                    posLoc, IM_COL32(255, 255, 255, 255), posString);
            }
        }

        if (selectedEntity && toolState != SceneControlPanel::ToolState::NONE)
        {
            if (selectedEntity.hasComponent<TransformCmp>())
            {
                ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::UNIVERSAL;
                switch (toolState)
                {
                    case (SceneControlPanel::ToolState::UNIVERSAL):
                    {
                        operation = ImGuizmo::OPERATION::UNIVERSAL;
                        break;
                    }
                    case (SceneControlPanel::ToolState::MOVE):
                    {
                        operation = ImGuizmo::OPERATION::TRANSLATE;
                        break;
                    }
                    case (SceneControlPanel::ToolState::SCALE):
                    {
                        operation = ImGuizmo::OPERATION::SCALE;
                        break;
                    }
                    case (SceneControlPanel::ToolState::ROTATE):
                    {
                        operation = ImGuizmo::OPERATION::ROTATE;
                        break;
                    }
                    default:
                    {
                        NM_ASSERT(false, "Unkown tool state %i", toolState);
                    }
                }

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
                                     operation,
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
    Application*                       mp_appRef;
    Window*                            mp_appWinRef;
    Camera*                            mp_editCamera;
    ref<Scene>                         mp_sceneContext;
    bool                               m_wasResized;
    ref<Framebuffer::PixelReadRequest> mp_pixelRequest;
    EntitySelectedCallback_t           m_entitySelectedCallback;
    bool                               m_requestedPixelVal = false;
    Entity                             m_selectionContext;
};

}  // namespace nimbus