#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/core/application.hpp"
#include "nimbus/core/window.hpp"
#include "nimbus/scene/camera.hpp"

#include "glm.hpp"

namespace nimbus
{

class EditCameraMenuPanel
{
   public:
    bool m_useEditorCamera = false;

    EditCameraMenuPanel(Camera* p_editCamera)
    {
        mp_appRef    = &Application::s_get();
        mp_appWinRef = &mp_appRef->getWindow();

        mp_editCamera   = p_editCamera;
    }
    ~EditCameraMenuPanel()
    {
    }

    void onDraw(const Camera::Bounds& worldBounds)
    {
        // Note this needs to be ran after IMGUI has generated
        // it's font atlas, the best way to ensure that has happened
        // is be inside a valid NewFrame. I.e. we can't do this
        // when we create this panel, as ImGui may still be
        // creating the atlas in a different thread.
        static bool fontSizeCalcd = false;
        if (!fontSizeCalcd)
        {
            m_textBaseWidth  = ImGui::CalcTextSize("A").x;
            m_textBaseHeight = ImGui::GetTextLineHeightWithSpacing();
            fontSizeCalcd    = true;
        }

        ImGui::Begin("Edit Camera Menu", 0);

        if (ImGui::Button("Reset Camera"))
        {
           _resetCamera();
        }

        const char* cameraTypes[] = {"Orthographic", "Perspective"};

        int currentType = static_cast<int>(mp_editCamera->getType());

        if (ImGui::Combo(
                "Type", &currentType, cameraTypes, IM_ARRAYSIZE(cameraTypes)))
        {
           if (currentType == 0)
           {
               mp_editCamera->setType(Camera::Type::ORTHOGRAPHIC);
           }
           else
           {
               mp_editCamera->setType(Camera::Type::PERSPECTIVE);
           }

           _resetCamera();
        }

        glm::vec3 cameraPos = mp_editCamera->getPosition();

        if (ImGui::DragFloat3("Position", glm::value_ptr(cameraPos), 0.1f))
        {
           mp_editCamera->setPosition(cameraPos);
        }

        float cameraSpeed = mp_editCamera->getSpeed();

        if (ImGui::DragFloat("Speed", &cameraSpeed, 0.1f, 0.0f, 50.0f))
        {
           mp_editCamera->setSpeed(cameraSpeed);
        }

        float farClip = mp_editCamera->getFarClip();
        if (ImGui::DragFloat("Far Clip", &farClip, 0.1f))
        {
           mp_editCamera->setFarClip(farClip);
        }

        float nearClip = mp_editCamera->getNearClip();
        if (ImGui::DragFloat("Near Clip", &nearClip, 0.1f))
        {
           mp_editCamera->setNearClip(nearClip);
        }

        ImGui::Spacing();

        if (ImGui::BeginTable("Camera Attributes",
                              4,
                              ImGuiTableFlags_SizingStretchSame
                                  | ImGuiTableFlags_NoHostExtendX))

        {
           ImGui::TableSetupColumn("Yaw", ImGuiTableColumnFlags_WidthStretch);
           ImGui::TableSetupColumn("Pitch", ImGuiTableColumnFlags_WidthStretch);
           ImGui::TableSetupColumn("Zoom", ImGuiTableColumnFlags_WidthStretch);
           ImGui::TableSetupColumn("FOV", ImGuiTableColumnFlags_WidthStretch);
           ImGui::TableHeadersRow();

           ImGui::TableNextRow();
           ImGui::TableNextColumn();
           ImGui::Text("%+.02f", mp_editCamera->getYaw());
           ImGui::TableNextColumn();
           ImGui::Text("%+.02f", mp_editCamera->getPitch());
           ImGui::TableNextColumn();
           ImGui::Text("%+.02f", mp_editCamera->getZoom());
           ImGui::TableNextColumn();
           ImGui::Text("%+.02f", mp_editCamera->getFov());

           ImGui::EndTable();
        }

        if (mp_editCamera->getType() == Camera::Type::ORTHOGRAPHIC)
        {
            if (ImGui::CollapsingHeader("Visible World Bounds"))
            {
               ImGui::BeginTable(
                   "Visible World Bounds", 2, ImGuiTableFlags_Borders);

               ImGui::SetNextItemWidth(m_textBaseWidth * 30);

               // Row 1
               ImGui::TableNextRow();

               // Column 1, Row 1
               ImGui::TableSetColumnIndex(0);
               ImGui::Text("%+.02f, %+.02f, %+.02f",
                           worldBounds.topLeft.x,
                           worldBounds.topLeft.y,
                           worldBounds.topLeft.z);

               // Column 2, Row 1
               ImGui::TableSetColumnIndex(1);
               ImGui::Text("%+.02f, %+.02f, %+.02f",
                           worldBounds.topRight.x,
                           worldBounds.topRight.y,
                           worldBounds.topRight.z);

               // Row 2
               ImGui::TableNextRow();

               // Column 1, Row 2
               ImGui::TableSetColumnIndex(0);
               ImGui::Text("%+.02f, %+.02f, %+.02f",
                           worldBounds.bottomLeft.x,
                           worldBounds.bottomLeft.y,
                           worldBounds.bottomLeft.z);

               // Column 2, Row 2
               ImGui::TableSetColumnIndex(1);
               ImGui::Text("%+.02f, %+.02f, %+.02f",
                           worldBounds.bottomRight.x,
                           worldBounds.bottomRight.y,
                           worldBounds.bottomLeft.z);

               // End the table
               ImGui::EndTable();
            }
        }

        ImGui::End();  // camera menu
    }

   private:
    Application* mp_appRef;
    Window*      mp_appWinRef;

    float m_textBaseWidth;
    float m_textBaseHeight;

    Camera* mp_editCamera;

    void _resetCamera()
    {
        if (mp_editCamera->getType() == Camera::Type::PERSPECTIVE)
        {
            mp_editCamera->setPosition({0.0f, 0.0f, 2.4125f});
            mp_editCamera->setYaw(-90.0f);
            mp_editCamera->setPitch(0.0f);
            mp_editCamera->setFov(45.0f);
        }
        else
        {
            mp_editCamera->setPosition({0.0f, 0.0f, 0.0f});
            mp_editCamera->setYaw(0.0f);
            mp_editCamera->setPitch(0.0f);
            mp_editCamera->setZoom(1.0f);
        }
    }
};

}  // namespace nimbus