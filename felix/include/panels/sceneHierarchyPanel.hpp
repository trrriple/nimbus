#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/core/application.hpp"
#include "nimbus/core/window.hpp"
#include "nimbus/scene/scene.hpp"
#include "nimbus/scene/entity.hpp"
#include "nimbus/scene/component.hpp"
#include "nimbus/core/resourceManager.hpp"
#include "nimbus/renderer/texture.hpp"
#include "IconsFontAwesome6.h"

#include <filesystem>

namespace nimbus
{

class SceneHeirarchyPanel
{
   private:
    ref<Texture> mp_checkerboardTex = nullptr;

   public:
    typedef std::function<void(Entity)> EntitySelectedCallback_t;

    SceneHeirarchyPanel(ref<Scene>& p_scene)
    {
        mp_appRef    = &Application::s_get();
        mp_appWinRef = &mp_appRef->getWindow();

        mp_sceneContext = p_scene;

        mp_checkerboardTex
            = Application::s_get().getResourceManager().loadTexture(
                Texture::Type::DIFFUSE,
                "../resources/textures/checkerboard.png");
    }
    ~SceneHeirarchyPanel()
    {
    }

    void setEntitySelectedCallback(const EntitySelectedCallback_t& callback)
    {
        m_entitySelectedCallback = callback;
    }

    void setSceneContext(ref<Scene>& p_scene)
    {
        mp_sceneContext = p_scene;
    }

    void onDraw(Entity selectedEntity)
    {
        ImGui::Begin("Heirarchy");

        if (ImGui::Button(ICON_FA_SQUARE_PLUS))
        {
            mp_sceneContext->addEntity();
            mp_sceneContext->sortEntities();
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add new entity");
            ImGui::EndTooltip();
        }

        m_selectionContext = selectedEntity;

        ImGui::SameLine();
        ImGuiTextFilter filter;
        filter.Draw(ICON_FA_FILTER);
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text(
                "'xxx' display lines containing 'xxx'\n 'xxx,yyy' display "
                "lines containing 'xxx' or 'yyy'\n '-xxx' hide lines "
                "containing 'xxx' ");
            ImGui::EndTooltip();
        }

        auto view  = mp_sceneContext->m_registry.view<GuidCmp>();
        int  count = view.size();

        std::vector<Entity> passedFilterEntities;
        passedFilterEntities.reserve(count);

        for (auto entityHandle : view)
        {
            Entity entity = {entityHandle, mp_sceneContext.raw()};
            auto&  name   = entity.getComponent<NameCmp>().name;

            if (filter.PassFilter(name.c_str()))
            {
                passedFilterEntities.push_back(entity);
            }
        }

        ImGui::Separator();

        ImGui::BeginChild("##ScrollRegion");

        ImGuiListClipper clipper;
        clipper.Begin(passedFilterEntities.size());
        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            {
                Entity& entity = passedFilterEntities[i];
                _drawEntity(entity);
            }
        }

        clipper.End();

        // clear selection when clicked off of in window
        if (m_selectionContext && ImGui::IsMouseDown(0)
            && ImGui::IsWindowHovered())
        {
            m_selectionContext = {};

            if (m_entitySelectedCallback)
            {
                m_entitySelectedCallback(m_selectionContext);
            }
        }
        ImGui::EndChild();


        // reduce default indentation for component panel
        float originalIndent            = ImGui::GetStyle().IndentSpacing;
        ImGui::GetStyle().IndentSpacing = originalIndent * 0.5f;

        ImGui::Begin("Properties");
        if (m_selectionContext)
        {
            _drawComponents(m_selectionContext);
        }
        ImGui::End();

        ImGui::GetStyle().IndentSpacing = originalIndent;

        ImGui::End();  // Heirarchy
    }

   private:
    Application* mp_appRef;
    Window*      mp_appWinRef;

    ref<Scene> mp_sceneContext;
    Entity     m_selectionContext;

    EntitySelectedCallback_t m_entitySelectedCallback;

    char m_scratch[1024];

    ////////////////////////////////////////////////////////////////////////////
    // Private Functions
    ////////////////////////////////////////////////////////////////////////////
    void _drawEntity(Entity& entity)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
                                   | ImGuiTreeNodeFlags_OpenOnDoubleClick
                                   | ImGuiTreeNodeFlags_SpanAvailWidth;

        // highlight the node if we've selected it
        flags |= ((m_selectionContext == entity) ? ImGuiTreeNodeFlags_Selected
                                                 : 0);

        auto& name = entity.getComponent<NameCmp>().name;

        bool open = ImGui::TreeNodeEx(
            (void*)entity.getId(), flags, ICON_FA_CUBES " %s", name.c_str());

        // select item as context if it's clicked
        if (ImGui::IsItemClicked(0))
        {
            m_selectionContext = entity;
            m_entitySelectedCallback(m_selectionContext);
        }

        bool deleted = false;
        // Check for right-click on the node
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Remove Entity"))
            {
                deleted = true;
            }

            ImGui::EndPopup();
        }

        if (open)
        {
            ImGui::Text("Blah blah blah");
            ImGui::TreePop();
        }

        if (deleted)
        {
            mp_sceneContext->removeEntity(entity);
            if (m_selectionContext == entity)
            {
                m_selectionContext = {};
                m_entitySelectedCallback(m_selectionContext);
            }
            mp_sceneContext->sortEntities();
        }
    }

    void _drawNameCmp(Entity& entity)
    {
        auto& name = entity.getComponent<NameCmp>().name;

        strncpy_s(m_scratch, name.c_str(), name.length());

        if (ImGui::InputText("##Name", m_scratch, sizeof(m_scratch)))
        {
            name = std::string(m_scratch);
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("GUID: %s \n CreationOrder %i",
                        entity.getComponent<GuidCmp>().guid.toString().c_str(),
                        entity.getComponent<GuidCmp>().creationOrder);
            ImGui::EndTooltip();
        }
    }

    void _drawSpriteCmp(Entity& entity)
    {
        static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth
                                          | ImGuiTreeNodeFlags_DefaultOpen;

        auto& spriteCmp = entity.getComponent<SpriteCmp>();

        if (ImGui::TreeNodeEx("Color", flags))
        {
            ImGui::ColorEdit4("Color",
                              glm::value_ptr(spriteCmp.color),
                              ImGuiColorEditFlags_Float
                                  | ImGuiColorEditFlags_AlphaBar
                                  | ImGuiColorEditFlags_AlphaPreview);

            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Texture", flags))
        {
            // Draw the loaded texture as a button image
            void* textureId = nullptr;
            if (spriteCmp.p_texture == nullptr)
            {
                textureId
                    = reinterpret_cast<void*>(mp_checkerboardTex->getId());
            }
            else
            {
                textureId
                    = reinterpret_cast<void*>(spriteCmp.p_texture->getId());
            }

            ImGui::BeginTable("Textures",
                              2,
                              ImGuiTableFlags_SizingStretchSame
                                  | ImGuiTableFlags_NoHostExtendX);
            ImGui::TableSetupColumn(
                "TexImage", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Options",
                                    ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (ImGui::ImageButton(textureId, {50, 50}))
            {
                // open the file dialog
                auto selection
                    = util::openFile("Select Texture to open",
                                     ".",
                                     {"Image Files",
                                      "*.png *.jpg *.jpeg *.bmp *.tga "
                                      "*.psd *.gif *.hdr *.pic *.pnm"},
                                     false);

                if (selection.size() != 0)
                {
                    // single file is selected
                    auto filePath = selection[0];

                    ref<Texture> texture
                        = Application::s_get().getResourceManager().loadTexture(
                            Texture::Type::DIFFUSE, filePath, false);

                    if (texture)
                    {
                        spriteCmp.p_texture = texture;
                    }
                    else
                    {
                        Log::warn("Could not load texture %s",
                                  filePath.c_str());
                    }
                }
            }
            // tooltip for button
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text(
                    "Drag and drop texture files here, or click to browse");
                ImGui::EndTooltip();
            }

            // support drag and drop files onto the image button
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload
                    = ImGui::AcceptDragDropPayload("DND_FILE"))
                {
                    const char*  path = (const char*)payload->Data;
                    ref<Texture> texture
                        = Application::s_get().getResourceManager().loadTexture(
                            Texture::Type::DIFFUSE, path, false);

                    if (texture)
                    {
                        spriteCmp.p_texture = texture;
                    }
                    else
                    {
                        Log::warn("Could not load texture %s", path);
                    }
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::TableNextColumn();
            ImGui::Text("Albedo");
            ImGui::DragFloat(
                "Tiling Factor", &spriteCmp.tilingFactor, 0.01f, 0.0f);

            ImGui::EndTable();

            ImGui::TreePop();
        }
    }

    void _drawTextCmp(Entity& entity)
    {
        static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth
                                          | ImGuiTreeNodeFlags_DefaultOpen;

        auto& textCmp = entity.getComponent<TextCmp>();

        if (ImGui::TreeNodeEx("Text", flags))
        {
            strncpy_s(m_scratch, textCmp.text.c_str(), textCmp.text.length());

            if (ImGui::InputTextMultiline(
                    "##Text", m_scratch, sizeof(m_scratch)))
            {
                textCmp.text = std::string(m_scratch);
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Format", flags))
        {
            // readonly flag prevents this from being dangerous
            std::string fontName;
            if (textCmp.format.p_font != nullptr)
            {
                fontName = textCmp.format.p_font->getPath().c_str();
            }
            else
            {
                fontName = "No font loaded";
            }

            // this const cast is safe because of the readonly flag
            ImGui::InputText("Font",
                             const_cast<char*>(fontName.c_str()),
                             fontName.length(),
                             ImGuiInputTextFlags_ReadOnly);

            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text(
                    "Drag and drop text files here, or double-click to browse");
                ImGui::EndTooltip();
            }
            // support drag and drop files onto the image button
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload
                    = ImGui::AcceptDragDropPayload("DND_FILE"))
                {
                    const char* filePath = (const char*)payload->Data;
                    ref<Font>   font
                        = Application::s_get().getResourceManager().loadFont(
                            filePath);

                    if (font)
                    {
                        textCmp.format.p_font = font;
                    }
                    else
                    {
                        Log::warn("Could not load font %s", filePath);
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(0))
            {
                auto selection = util::openFile("Select Font to open",
                                                ".",
                                                {"Font Files", "*.ttf *.otf"},
                                                false);
                if (selection.size() != 0)
                {
                    // single file is selected
                    auto filePath = selection[0];

                    ref<Font> font
                        = Application::s_get().getResourceManager().loadFont(
                            filePath);

                    if (font)
                    {
                        textCmp.format.p_font = font;
                    }
                    else
                    {
                        Log::warn("Could not load font %s", filePath.c_str());
                    }
                }
            }

            // show when the font is loading
            if (textCmp.format.p_font != nullptr)
            {
                if (!textCmp.format.p_font->isLoaded())
                {
                    static const std::vector<std::string> anim = {
                        "",
                        ".",
                        "..",
                        "...",
                    };
                    ImGui::Text(
                        "Generating Font Atlas%s",
                        anim[(int)(ImGui::GetTime() / 0.25f) & 3].c_str());
                }
            }

            ImGui::ColorEdit4("Fg Color",
                              glm::value_ptr(textCmp.format.fgColor),
                              ImGuiColorEditFlags_Float
                                  | ImGuiColorEditFlags_AlphaBar
                                  | ImGuiColorEditFlags_AlphaPreview);

            ImGui::ColorEdit4("Bg Color",
                              glm::value_ptr(textCmp.format.bgColor),
                              ImGuiColorEditFlags_Float
                                  | ImGuiColorEditFlags_AlphaBar
                                  | ImGuiColorEditFlags_AlphaPreview);

            ImGui::DragFloat("Kerning", &textCmp.format.kerning, 0.01f);
            ImGui::DragFloat("Leading", &textCmp.format.leading, 0.01f);

            ImGui::TreePop();
        }
    }

    void _drawCameraCmp(Entity& entity)
    {
        auto& cameraCmp = entity.getComponent<CameraCmp>();

        const char* cameraTypes[] = {"Orthographic", "Perspective"};

        int currentType = static_cast<int>(cameraCmp.camera.getType());
        if (ImGui::Combo(
                "Type", &currentType, cameraTypes, IM_ARRAYSIZE(cameraTypes)))
        {
            if (currentType == 0)
            {
                cameraCmp.camera.setType(Camera::Type::ORTHOGRAPHIC);
            }
            else
            {
                cameraCmp.camera.setType(Camera::Type::PERSPECTIVE);
            }
        }

        bool primary = cameraCmp.primary;
        if (ImGui::Checkbox("Primary", &primary))
        {
            // if this is the primary camera, disable primary
            // for any other camera that has it
            if (primary)
            {
                auto cameraView = mp_sceneContext->m_registry.view<CameraCmp>();

                for (auto entity : cameraView)
                {
                    auto& cameraCmp2 = cameraView.get<CameraCmp>(entity);

                    cameraCmp2.primary = false;
                }
            }
        }

        // now set this one to the select state
        // important this happens after because we just cleared
        // the primary state on all camera components
        cameraCmp.primary = primary;

        if (ImGui::Checkbox("Fixed Aspect", &cameraCmp.fixedAspect))
        {
            if (!cameraCmp.fixedAspect)
            {
                cameraCmp.camera.setAspectRatio(mp_sceneContext->m_aspectRatio);
            }
        }

        if (cameraCmp.fixedAspect)
        {
            float aspect = cameraCmp.camera.getAspectRatio();
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100.0f);
            if (ImGui::DragFloat("##Aspect", &aspect, .001f))
            {
                cameraCmp.camera.setAspectRatio(aspect);
            }
        }

        auto position = cameraCmp.camera.getPosition();

        bool locked = false;
        if (_drawVec3Control("Position", position, 0.0f, 0.01f, false, locked))
        {
            cameraCmp.camera.setPosition(position);
        }

        if (currentType == 0)
        {
            float zoom = cameraCmp.camera.getZoom();
            if (ImGui::DragFloat("Zoom", &zoom, 0.005f))
            {
                cameraCmp.camera.setZoom(zoom);
            }
        }
        else
        {
            float fov = cameraCmp.camera.getFov();
            if (ImGui::DragFloat("FOV", &fov, 0.1f))
            {
                cameraCmp.camera.setZoom(fov);
            }
        }

        float farClip = cameraCmp.camera.getFarClip();
        if (ImGui::DragFloat("Far Clip", &farClip, 0.1f))
        {
            cameraCmp.camera.setFarClip(farClip);
        }

        float nearClip = cameraCmp.camera.getNearClip();
        if (ImGui::DragFloat("Near Clip", &nearClip, 0.1f))
        {
            cameraCmp.camera.setNearClip(nearClip);
        }
    }

    void _drawTransformCmp(Entity& entity)
    {
        auto& transformCmp = entity.getComponent<TransformCmp>();

        glm::vec3 translation = transformCmp.getTranslation();

        bool locked = false;
        if (_drawVec3Control(
                "Translation", translation, 0.0f, 0.01f, false, locked))
        {
            transformCmp.setTranslation(translation);
        }

        glm::vec3 rotation = glm::degrees(transformCmp.getRotation());

        if (_drawVec3Control("Rotation", rotation, 0.0f, 0.1f, false, locked))
        {
            transformCmp.setRotation(glm::radians(rotation));
        }

        glm::vec3 scale = transformCmp.getScale();

        locked = transformCmp.isScaleLocked();
        if (_drawVec3Control("Scale", scale, 1.0f, 0.01f, true, locked))
        {
            transformCmp.setScaleLocked(locked);

            if (transformCmp.isScaleLocked())
            {
                // check what changed
                if (scale.x != transformCmp.getScale().x)
                {
                    transformCmp.setScaleX(scale.x);
                }
                else if (scale.y != transformCmp.getScale().y)
                {
                    transformCmp.setScaleY(scale.y);
                }
                else if (scale.z != transformCmp.getScale().z)
                {
                    transformCmp.setScaleZ(scale.z);
                }
            }
            else
            {
                transformCmp.setScale(scale);
            }
        }
    }

    void _drawComponents(Entity& entity)
    {
        static ImGuiTreeNodeFlags flags
            = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen
              | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap;

        ///////////////////////////
        // Name  Component
        ///////////////////////////
        // no tree node for the name
        _drawNameCmp(entity);

        ImGui::SameLine();
        if (ImGui::Button("Add " ICON_FA_CARET_DOWN))
        {
            ImGui::OpenPopup("addComponent");
        }

        if (ImGui::BeginPopup("addComponent"))
        {
            if (_drawAddComponentEntry<SpriteCmp>(ICON_FA_METEOR " Sprite"))
            {
                // add Transform component that will probably
                // be desired
                if (!entity.hasComponent<TransformCmp>())
                {
                    entity.addComponent<TransformCmp>();
                }
            }

            if (_drawAddComponentEntry<TextCmp>(ICON_FA_FONT " Text"))
            {
                if (!entity.hasComponent<TransformCmp>())
                {
                    entity.addComponent<TransformCmp>();
                }
            }

            _drawAddComponentEntry<NativeLogicCmp>(ICON_FA_COMPUTER
                                                   " Native Logic");
            _drawAddComponentEntry<TransformCmp>(ICON_FA_ATOM " Transform");
            _drawAddComponentEntry<CameraCmp>(ICON_FA_VIDEO " Camera");

            ImGui::EndPopup();
        }

        ///////////////////////////
        // Sprite Component
        ///////////////////////////
        if (entity.hasComponent<SpriteCmp>())
        {
            bool open = ImGui::TreeNodeEx(ICON_FA_METEOR " Sprite", flags);

            bool removed = _drawComponentMenu();

            if (open)
            {
                _drawSpriteCmp(entity);
                ImGui::TreePop();
            }

            if (removed)
            {
                entity.removeComponent<SpriteCmp>();
            }
        }

        ///////////////////////////
        // Text Component
        ///////////////////////////
        if (entity.hasComponent<TextCmp>())
        {
            bool open    = ImGui::TreeNodeEx(ICON_FA_FONT " Text", flags);
            bool removed = _drawComponentMenu();

            if (open)
            {
                _drawTextCmp(entity);
                ImGui::TreePop();
            }

            if (removed)
            {
                entity.removeComponent<TextCmp>();
            }
        }

        ///////////////////////////
        // Camera Component
        ///////////////////////////
        if (entity.hasComponent<CameraCmp>())
        {
            bool open    = ImGui::TreeNodeEx(ICON_FA_VIDEO " Camera", flags);
            bool removed = _drawComponentMenu();

            if (open)
            {
                _drawCameraCmp(entity);
                ImGui::TreePop();
            }

            if (removed)
            {
                entity.removeComponent<CameraCmp>();
            }
        }

        ///////////////////////////
        // Transform Component
        ///////////////////////////
        if (entity.hasComponent<TransformCmp>())
        {
            bool open    = ImGui::TreeNodeEx(ICON_FA_ATOM " Transform", flags);
            bool removed = _drawComponentMenu();

            if (open)
            {
                _drawTransformCmp(entity);
                ImGui::TreePop();
            }

            if (removed)
            {
                entity.removeComponent<TransformCmp>();
            }
        }
    }

    // custom stylized 3 input (X,Y,Z) control block
    static bool _drawVec3Control(const std::string& label,
                                 glm::vec3&         values,
                                 float              resetValue,
                                 float              speed,
                                 bool               lockable,
                                 bool&              lockedStatus)
    {
        // set the size of the dragfloats
        static float itemWidth = ImGui::CalcTextSize("A").x * 6.0;

        static float lineHeight
            = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;

        static ImVec2 buttonSize = {lineHeight - 3, lineHeight};

        ImGuiIO& io       = ImGui::GetIO();
        auto     boldfont = io.Fonts->Fonts[1];

        // set the size of the buttons
        bool changedX = false;
        bool changedY = false;
        bool changedZ = false;

        ImGui::BeginTable("table", 2, ImGuiTableFlags_SizingFixedFit);
        ImGui::TableSetupColumn("Data",
                                ImGuiTableColumnFlags_WidthFixed,
                                (itemWidth * 3 + buttonSize.x * 3));
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4{0.8f, 0.1f, 0.15f, 1.0f});

        ImGui::PushFont(boldfont);
        if (ImGui::Button("X", buttonSize))
        {
            values.x = resetValue;
            changedX = true;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::PushItemWidth(itemWidth);
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

        ImGui::PushFont(boldfont);
        if (ImGui::Button("Y", buttonSize))
        {
            values.y = resetValue;
            changedY = true;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::PushItemWidth(itemWidth);
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

        ImGui::PushFont(boldfont);
        if (ImGui::Button("Z", buttonSize))
        {
            values.z = resetValue;
            changedZ = true;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::PushItemWidth(itemWidth);
        changed = ImGui::DragFloat("##Z", &values.z, speed, 0.0f, 0.0f, "%.2f");
        if (changed)
            changedZ = true;

        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::TableNextColumn();
        ImGui::Text("%s", label.c_str());

        bool changedLock = false;
        if (lockable)
        {
            ImGui::SameLine();

            if (ImGui::Button(lockedStatus ? ICON_FA_LOCK : ICON_FA_LOCK_OPEN))
            {
                lockedStatus = !lockedStatus;
                changedLock  = true;
            }
        }

        ImGui::EndTable();

        return changedX || changedY || changedZ || changedLock;
    }

    template <typename T>
    bool _drawAddComponentEntry(const std::string& entryName)
    {
        bool added = false;
        if (!m_selectionContext.hasComponent<T>())
        {
            if (ImGui::MenuItem(entryName.c_str()))
            {
                m_selectionContext.addComponent<T>();
                added = true;
                ImGui::CloseCurrentPopup();
            }
        }
        return added;
    }

    bool _drawComponentMenu()
    {
        bool removed = false;
        ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);

        if (ImGui::Button(ICON_FA_BARS))
        {
            ImGui::OpenPopup("componentMenu");
        }

        if (ImGui::BeginPopup("componentMenu"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                removed = true;
            }

            ImGui::EndPopup();
        }

        return removed;
    }
};

}  // namespace nimbus