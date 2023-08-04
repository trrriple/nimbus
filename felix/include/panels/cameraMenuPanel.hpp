#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/core/application.hpp"
#include "nimbus/core/window.hpp"
#include "nimbus/scene/camera.hpp"

#include "glm.hpp"

namespace nimbus
{

class CameraMenuPanel
{
   public:
    bool m_useEditorCamera = false;

    CameraMenuPanel(Camera* p_sceneCamera, Camera* p_editCamera)
    {
        mp_appRef    = &Application::s_get();
        mp_appWinRef = &mp_appRef->getWindow();

        mp_sceneCamera = p_sceneCamera;
        mp_editCamera  = p_editCamera;
    }
    ~CameraMenuPanel()
    {
    }

    void onDraw(Camera*               p_currCamera,
                const Camera::Bounds& bounds,
                const glm::vec2&      viewPortSize)
    {
        ImGui::Begin("Camera Menu", 0);

        m_useEditorCamera = p_currCamera == mp_editCamera;
        ImGui::Checkbox("Editor Camera", &m_useEditorCamera);

        ImGui::SameLine();

        if (ImGui::Button("Reset Camera"))
        {
            if (p_currCamera == mp_editCamera)
            {
                p_currCamera->setPosition({0.0f, 0.0f, 2.4125f});
                p_currCamera->setYaw(-90.0f);
                p_currCamera->setPitch(0.0f);
                p_currCamera->setFov(45.0f);
            }
            else
            {
                p_currCamera->setPosition({0.0f, 0.0f, 0.0f});
                p_currCamera->setYaw(0.0f);
                p_currCamera->setPitch(0.0f);
                p_currCamera->setZoom(1.0f);
            }
        }

        ImGui::Text("Viewport dimensions %i, %i (%f)",
                    (int)viewPortSize.x,
                    (int)viewPortSize.y,
                    p_currCamera->getAspectRatio());

        glm::vec3 cameraPos = p_currCamera->getPosition();

        if (ImGui::DragFloat3("Camera Pos", glm::value_ptr(cameraPos), 0.1f))
        {
            p_currCamera->setPosition(cameraPos);
        }

        float cameraSpeed = p_currCamera->getSpeed();

        if (ImGui::DragFloat("Camera Speed", &cameraSpeed, 0.1f, 0.0f, 50.0f))
        {
            p_currCamera->setSpeed(cameraSpeed);
        }

        ImGui::Spacing();

        ImGui::Text("Pitch %.02f, Yaw %.02f, Zoom %.02f, FOV %.02f",
                    p_currCamera->getPitch(),
                    p_currCamera->getYaw(),
                    p_currCamera->getZoom(),
                    p_currCamera->getFov());

        if (ImGui::CollapsingHeader("Visible World Bounds"))
        {
            ImGui::BeginTable(
                "Visible World Bounds", 2, ImGuiTableFlags_Borders);

            // Row 1
            ImGui::TableNextRow();

            // Column 1, Row 1
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("X:%.02f, Y:%.02f, Z:%.02f",
                        bounds.topLeft.x,
                        bounds.topLeft.y,
                        bounds.topLeft.z);

            // Column 2, Row 1
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("X:%.02f, Y:%.02f, Z:%.02f",
                        bounds.topRight.x,
                        bounds.topRight.y,
                        bounds.topRight.z);

            // Row 2
            ImGui::TableNextRow();

            // Column 1, Row 2
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("X:%.02f, Y:%.02f, Z:%.02f",
                        bounds.bottomLeft.x,
                        bounds.bottomLeft.y,
                        bounds.bottomLeft.z);

            // Column 2, Row 2
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("X:%.02f, Y:%.02f, Z:%.02f",
                        bounds.bottomRight.x,
                        bounds.bottomRight.y,
                        bounds.bottomLeft.z);

            // End the table
            ImGui::EndTable();
        }

        ImGui::End();  // camera menu
    }

   private:
    Application* mp_appRef;
    Window*      mp_appWinRef;

    Camera* mp_sceneCamera;
    Camera* mp_editCamera;
};

}  // namespace nimbus