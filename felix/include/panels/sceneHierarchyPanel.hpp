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
        
        ///////////////////////////
        // Name  Component
        ///////////////////////////
        if(entity.hasComponent<NameCmp>())
        {
            if (ImGui::TreeNodeEx("Name", ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto& name = entity.getComponent<NameCmp>().name;

                strncpy_s(m_scratch, name.c_str(), name.length());

                if (ImGui::InputText("##Name", m_scratch, sizeof(m_scratch)))
                {
                    name = std::string(m_scratch);
                }
                ImGui::TreePop();
            }
        }

        ///////////////////////////
        // Sprite Component
        ///////////////////////////
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

        ///////////////////////////
        // Text Component
        ///////////////////////////
        if (entity.hasComponent<TextCmp>())
        {
            auto& textCmp = entity.getComponent<TextCmp>();

            if (ImGui::TreeNodeEx("Text", ImGuiTreeNodeFlags_DefaultOpen))
            {
                strncpy_s(
                    m_scratch, textCmp.text.c_str(), textCmp.text.length());

                if (ImGui::InputTextMultiline(
                        "##Text", m_scratch, sizeof(m_scratch)))
                {
                    textCmp.text = std::string(m_scratch);
                }

                ImGui::TreePop();
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

        ///////////////////////////
        // Camera Component
        ///////////////////////////
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


                bool primary = cameraCmp.primary;
                if (ImGui::Checkbox("Primary", &primary))
                {
                    // if this is the primary camera, disable primary
                    // for any other camera that has it
                    if (primary)
                    {
                        auto cameraView
                            = mp_sceneContext->m_registry.view<CameraCmp>();

                        for (auto entity : cameraView)
                        {
                            auto& cameraCmp2
                                = cameraView.get<CameraCmp>(entity);

                            cameraCmp2.primary = false;
                        }
                    }
                }

                // now set this one to the select state
                // important this happens after because we just cleared
                // the primary state on all camera components
                cameraCmp.primary = primary;

                ImGui::Checkbox("Fixed Aspect", &cameraCmp.fixedAspect);

                if (cameraCmp.fixedAspect)
                {
                    float aspect = cameraCmp.p_camera->getAspectRatio();
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(100.0f);
                    if (ImGui::DragFloat("##Aspect", &aspect, .001f))
                    {
                        cameraCmp.p_camera->setAspectRatio(aspect);
                    }
                }

                auto position = cameraCmp.p_camera->getPosition();
                if (ImGui::DragFloat3(
                        "Position", glm::value_ptr(position), 0.01f))
                {
                    cameraCmp.p_camera->setPosition(position);
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

                ImGui::TreePop();
            }
        }

        ///////////////////////////
        // Transform Component
        ///////////////////////////
        if (entity.hasComponent<TransformCmp>())
        {
            auto& transformCmp = entity.getComponent<TransformCmp>();

            if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                glm::vec3 translation = transformCmp.getTranslation();

                if (_drawVec3Control("Translation", translation, 0.0f, 0.01f))
                {
                    transformCmp.setTranslation(translation);
                }

                glm::vec3 rotation = glm::degrees(transformCmp.getRotation());

                if (_drawVec3Control("Rotation", rotation, 0.0f, 0.1f))
                {
                    transformCmp.setRotation(glm::radians(rotation));
                }

                glm::vec3 scale = transformCmp.getScale();

                if (_drawVec3Control("Scale", scale, 1.0f, 0.01f))
                {
                    transformCmp.setScale(scale);
                }

                ImGui::TreePop();
            }
        }
    }

    static bool _drawVec3Control(const std::string& label,
                                 glm::vec3&         values,
                                 float              resetValue = 0.0f,
                                 float              speed      = 0.1f)
    {
        bool changedX = false;
        bool changedY = false;
        bool changedZ = false;
        
        ImGui::BeginTable("table", 2, ImGuiTableFlags_SizingFixedFit);
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        float itemWidth = ImGui::CalcItemWidth() + 20;
        float lineHeight
            = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;

        ImVec2 buttonSize = {lineHeight - 3, lineHeight};
        
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        if (ImGui::Button(ICON_FA_X, buttonSize))
        {
            values.x = resetValue;
            changedX = true;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::PushItemWidth(itemWidth / 3);
        bool changed
            = ImGui::DragFloat("##X", &values.x, speed, 0.0f, 0.0f, "%.2f");
        if (changed)
            changedX = true;

        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        if (ImGui::Button(ICON_FA_Y, buttonSize))
        {
            values.y = resetValue;
            changedY = true;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::PushItemWidth(itemWidth / 3);
        changed = ImGui::DragFloat("##Y", &values.y, speed, 0.0f, 0.0f, "%.2f");
        if (changed)
            changedY = true;

        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
        if (ImGui::Button(ICON_FA_Z, buttonSize))
        {
            values.z = resetValue;
            changedZ = true;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::PushItemWidth(itemWidth / 3);
        changed = ImGui::DragFloat("##Z", &values.z, speed, 0.0f, 0.0f, "%.2f");
        if (changed)
            changedZ = true;

        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::TableNextColumn();
        ImGui::Text("%s", label.c_str());

        ImGui::EndTable();

        return changedX || changedY || changedZ;
    }
};

}  // namespace nimbus