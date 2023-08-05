#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/core/application.hpp"
#include "nimbus/core/window.hpp"
#include "nimbus/scene/scene.hpp"
#include "nimbus/scene/entity.hpp"
#include "nimbus/scene/component.hpp"
#include "IconsFontAwesome6.h"

namespace nimbus
{

class SceneHeirarchyPanel
{
   public:

    SceneHeirarchyPanel(ref<Scene>& p_scene)
    {
        mp_appRef    = &Application::s_get();
        mp_appWinRef = &mp_appRef->getWindow();

        mp_sceneContext = p_scene;


    }
    ~SceneHeirarchyPanel()
    {
    }

    void setSceneContext(ref<Scene>& p_scene)
    {
        mp_sceneContext = p_scene;
    }

    void onDraw()
    {
        ImGui::Begin("Heirarchy");
        
        for (auto [entityHandle] :
             mp_sceneContext->m_registry.storage<entt::entity>().each())
        {
            Entity entity = {entityHandle, mp_sceneContext.get()};
            _drawNode(entity);
        }

        // clear selection when clicked off of in window
        if (m_selectionContext &&
            ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        {
            m_selectionContext = {};
        }

        ImGui::Begin("Properties");
        if (m_selectionContext)
        {
            _drawComponents(m_selectionContext);
        }
        ImGui::End();
 

        ImGui::End(); // Heirarchy
    }

   private:
    Application* mp_appRef;
    Window*      mp_appWinRef;

    ref<Scene> mp_sceneContext;
    Entity     m_selectionContext;

    char m_scratch[1024];

    void _drawNode(Entity entity)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
                                   | ImGuiTreeNodeFlags_OpenOnDoubleClick
                                   | ImGuiTreeNodeFlags_SpanAvailWidth;

        // highlight the node if we've selected it
        flags |= ((m_selectionContext == entity) ? ImGuiTreeNodeFlags_Selected
                                                 : 0);

        auto name = entity.getComponent<NameCmp>().name;

        if (ImGui::TreeNodeEx(
                (void*)entity.getId(), flags, ICON_FA_CUBES " %s", name.c_str()))
        {
            ImGui::TreePop();
        }

        if (ImGui::IsItemClicked())
        {
            m_selectionContext = entity;
        }
    }

    void _drawComponents(Entity entity)
    {
        if(entity.hasComponent<NameCmp>())
        {
            auto& name = entity.getComponent<NameCmp>().name;

            strncpy_s(m_scratch, name.c_str(), name.length());

            if (ImGui::InputText("Name", m_scratch, sizeof(m_scratch)))
            {
                name = std::string(m_scratch);
            }
        }

        if (entity.hasComponent<TextCmp>())
        {
            auto& textCmp = entity.getComponent<TextCmp>();

            strncpy_s(m_scratch, textCmp.text.c_str(),  textCmp.text.length());

            if (ImGui::InputTextMultiline("Text", m_scratch, sizeof(m_scratch)))
            {
                textCmp.text = std::string(m_scratch);
            }

            if (ImGui::TreeNodeEx("Format", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::ColorEdit4("Fg Color",
                                  glm::value_ptr(textCmp.format.fgColor),
                                  ImGuiColorEditFlags_AlphaBar
                                      | ImGuiColorEditFlags_AlphaPreview);

                ImGui::ColorEdit4("Bg Color",
                                  glm::value_ptr(textCmp.format.bgColor),
                                  ImGuiColorEditFlags_AlphaBar
                                      | ImGuiColorEditFlags_AlphaPreview);

                ImGui::DragFloat("Kerning", &textCmp.format.kerning, 0.01f);
                ImGui::DragFloat(
                    "Leading", &textCmp.format.lineSpacing, 0.01f);

                ImGui::TreePop();
            }
        }

        if (entity.hasComponent<TransformCmp>())
        {
            auto& transformCmp = entity.getComponent<TransformCmp>();

            if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                glm::vec3 translation = transformCmp.getTranslation();

                if (ImGui::DragFloat3(
                        "Translation", glm::value_ptr(translation), 0.01f))
                {
                    transformCmp.setTranslation(translation);
                }

                glm::vec3 rotation = transformCmp.getRotation();

                if (ImGui::DragFloat3(
                        "Rotation", glm::value_ptr(rotation), 0.01f))
                {
                    transformCmp.setRotation(rotation);
                }

                glm::vec3 scale = transformCmp.getScale();

                if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f))
                {
                    transformCmp.setScale(scale);
                }

                ImGui::TreePop();
            }
        }

        if (entity.hasComponent<SpriteCmp>())
        {
            auto& spriteCmp = entity.getComponent<SpriteCmp>();

            if (ImGui::TreeNodeEx("Color", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::ColorEdit4("Color",
                                  glm::value_ptr(spriteCmp.color),
                                  ImGuiColorEditFlags_AlphaBar
                                      | ImGuiColorEditFlags_AlphaPreview);

                ImGui::TreePop();
            }
        }


        if (entity.hasComponent<CameraCmp>())
        {
            auto& cameraCmp = entity.getComponent<CameraCmp>();

            if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_DefaultOpen))
            {
                const char* cameraTypes[] = {"Orthographic", "Perspective"};

                int currentType
                    = static_cast<int>(cameraCmp.p_camera->getType());

                if (ImGui::Combo("Type",
                                 &currentType,
                                 cameraTypes,
                                 IM_ARRAYSIZE(cameraTypes)))
                {
                    if (currentType == 0)
                    {
                        cameraCmp.p_camera->setType(Camera::Type::ORTHOGRAPHIC);
                    }
                    else
                    {
                        cameraCmp.p_camera->setType(Camera::Type::PERSPECTIVE);
                    }
                }

                if (currentType == 0)
                {
                    float zoom = cameraCmp.p_camera->getZoom();
                    if (ImGui::DragFloat("Zoom", &zoom, 0.005f))
                    {
                        cameraCmp.p_camera->setZoom(zoom);
                    }
                }
                else
                {
                    float fov = cameraCmp.p_camera->getFov();
                    if (ImGui::DragFloat("FOV", &fov, 0.1f))
                    {
                        cameraCmp.p_camera->setZoom(fov);
                    }
                }

                float farClip = cameraCmp.p_camera->getFarClip();
                if(ImGui::DragFloat("Far Clip", &farClip, 0.1f))
                {
                    cameraCmp.p_camera->setFarClip(farClip);
                }


                float nearClip = cameraCmp.p_camera->getNearClip();
                if(ImGui::DragFloat("Near Clip", &nearClip, 0.1f))
                {
                    cameraCmp.p_camera->setNearClip(nearClip);
                }

                auto position = cameraCmp.p_camera->getPosition();
                ImGui::Checkbox("Primary", &cameraCmp.primary);

                ImGui::Checkbox("Fixed Aspect", &cameraCmp.fixedAspect);

                if(cameraCmp.fixedAspect)
                {
                    float aspect = cameraCmp.p_camera->getAspectRatio();
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(100.0f);
                    if (ImGui::DragFloat("##Aspect", &aspect, .001f))
                    {
                        cameraCmp.p_camera->setAspectRatio(aspect);
                    }
                }

                if (ImGui::DragFloat3(
                        "Position", glm::value_ptr(position), 0.01f))
                {
                    cameraCmp.p_camera->setPosition(position);
                }

                ImGui::TreePop();
            }
        }
    }
};

}  // namespace nimbus