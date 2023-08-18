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

    SceneHeirarchyPanel(ref<Scene> p_scene)
    {
        mp_appRef    = &Application::s_get();
        mp_appWinRef = &mp_appRef->getWindow();

        mp_sceneContext = p_scene;

        mp_checkerboardTex = Application::s_get().getResourceManager().loadTexture(
            Texture::Type::DIFFUSE, "../resources/textures/checkerboard.png");
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
            selectedEntity = mp_sceneContext->addEntity();
            m_entitySelectedCallback(selectedEntity);
            mp_sceneContext->sortEntities();
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Add new entity");
            ImGui::EndTooltip();
        }

        ImGui::SameLine();
        static ImGuiTextFilter filter;

        static bool selectionScrollHandled = true;

        if (selectedEntity != m_selectionContext)
        {
            m_selectionContext = selectedEntity;

            selectionScrollHandled = false;
            filter.Clear();
        }

        bool filterActive = filter.IsActive();

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

        ImGui::SameLine();
        if (ImGui::Button("Clear"))
        {
            filter.Clear();
        }

        ImGui::Separator();
        ImGui::BeginChild("##ScrollRegion");

        static float preFilterScrollPos = 0.0f;
        static bool  scrollResetHandled = true;
        if (!filterActive && scrollResetHandled)
        {
            preFilterScrollPos = ImGui::GetScrollY();
        }

        // handle resetting scroll position after finishing a filter
        if ((filterActive && scrollResetHandled))
        {
            scrollResetHandled = false;
        }
        else if (!filterActive && !scrollResetHandled)
        {
            ImGui::SetScrollY(preFilterScrollPos);
            scrollResetHandled = true;
        }

        auto view  = mp_sceneContext->m_registry.view<GuidCmp>();
        int  count = view.size();

        std::vector<Entity> passedFilterEntities;
        passedFilterEntities.reserve(count);

        int selectedIdx = -1;
        for (int i = 0; i < count; i++)
        {
            auto   entityHandle = view[i];
            Entity entity       = {entityHandle, mp_sceneContext.raw()};
            auto&  name         = entity.getComponent<NameCmp>().name;

            if (filter.PassFilter(name.c_str()) && !entity.getComponent<AncestryCmp>().parent)
            {
                // only draw entities that passed the filter and are
                // top level entities
                passedFilterEntities.push_back(entity);
            }

            if (entity == m_selectionContext && !selectionScrollHandled)
            {
                if (!selectionScrollHandled && !filterActive)
                {
                    // selected entity must be in list in order to scroll to it
                    selectedIdx = i;
                }
            }
        }

        // clipper allows us to only proceess and render entities that can fit
        // in the current scroll area of the panel. This is a significant
        // performance improvement when there are lots (thousands) of entities
        // because a. they don't need to be processed by imgui and b. they
        // don't need to be processed by us.
        ImGuiListClipper clipper;
        clipper.Begin(passedFilterEntities.size(), 20.0f);
        if (selectedIdx != -1)
        {
            // selected entity must be in list in order to scroll to it
            clipper.IncludeRangeByIndices(selectedIdx, selectedIdx + 1);
        }
        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            {
                Entity& entity = passedFilterEntities[i];

                Entity selectedEntity = _drawEntity(entity);

                if (selectedEntity)  // selection changed
                {
                    if (selectedEntity != m_selectionContext)
                    {
                        m_selectionContext = selectedEntity;
                        m_entitySelectedCallback(m_selectionContext);

                        if (filterActive)
                        {
                            selectionScrollHandled = false;
                        }
                    }
                }

                if (entity == m_selectionContext)
                {
                    if (!selectionScrollHandled && !filterActive)
                    {
                        ImGui::SetScrollHereY();
                        selectionScrollHandled = true;
                    }
                }
            }
        }

        clipper.End();

        ImGui::EndChild();

        // reduce default indentation for component panel
        float originalIndent            = ImGui::GetStyle().IndentSpacing;
        ImGui::GetStyle().IndentSpacing = originalIndent * 0.25f;

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
    Entity _drawEntity(Entity& entity)
    {
        Entity selectedEntity;

        auto& name = entity.getComponent<NameCmp>().name;
        auto& ac   = entity.getComponent<AncestryCmp>();

        bool isChild     = ac.parent;
        bool hasChildren = ac.children.size() > 0;

        ImGuiTreeNodeFlags flags
            = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

        // highlight the node if we've selected it
        flags |= ((m_selectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0);

        // make this a leaf if it doesn't have children
        flags |= hasChildren ? 0 : ImGuiTreeNodeFlags_Leaf;

        auto icon = (isChild ? ICON_FA_CUBE : ICON_FA_CUBES);

        // if the section context is a child of this, force it to be open

        for (auto child : ac.children)
        {
            if (m_selectionContext == child)
            {
                ImGui::SetNextItemOpen(true);
                break;
            }
        }

        bool open = ImGui::TreeNodeEx((void*)entity.getId(), flags, "%s %s", icon, name.c_str());

        // select item as context if it's clicked
        if (ImGui::IsItemClicked(0))
        {
            selectedEntity = entity;
        }

        Entity selectedChild;
        bool   deleted = false;
        // Check for right-click on the node
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Remove Entity"))
            {
                deleted = true;
            }

            if (ImGui::MenuItem("Add Child Entity"))
            {
                selectedChild = mp_sceneContext->addChildEntity(entity);
            }

            ImGui::EndPopup();
        }

        if (open)
        {
            // draw children
            for (uint32_t i = 0; i < ac.children.size(); i++)
            {
                Entity selected = _drawEntity(ac.children[i]);

                if (selected)
                {
                    selectedChild = selected;
                }
            }

            ImGui::TreePop();
        }

        if (deleted)
        {
            // if a child, remove all of it's children too
            mp_sceneContext->removeEntity(entity, isChild);
            if (m_selectionContext == entity)
            {
                m_selectionContext = {};
                m_entitySelectedCallback(m_selectionContext);
            }
            mp_sceneContext->sortEntities();
        }

        return selectedChild ? selectedChild : selectedEntity;
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
            ImGui::Text("GUID: %s \n sequenceIndex %i",
                        entity.getComponent<GuidCmp>().guid.toString().c_str(),
                        entity.getComponent<GuidCmp>().sequenceIndex);
            ImGui::EndTooltip();
        }
    }

    void _drawSpriteCmp(Entity& entity)
    {
        static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

        auto& spriteCmp = entity.getComponent<SpriteCmp>();

        if (ImGui::TreeNodeEx("Color", flags))
        {
            ImGui::ColorEdit4(
                "Color",
                glm::value_ptr(spriteCmp.color),
                ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);

            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Texture", flags))
        {
            // Draw the loaded texture as a button image
            void* textureId = nullptr;
            if (spriteCmp.p_texture == nullptr)
            {
                textureId = reinterpret_cast<void*>(mp_checkerboardTex->getId());
            }
            else
            {
                textureId = reinterpret_cast<void*>(spriteCmp.p_texture->getId());
            }

            ImGui::BeginTable("Textures", 2, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoHostExtendX);
            ImGui::TableSetupColumn("TexImage", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Options", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (ImGui::ImageButton(textureId, {50, 50}))
            {
                // open the file dialog
                auto selection = util::openFile("Select Texture to open",
                                                ".",
                                                {"Image Files",
                                                 "*.png *.jpg *.jpeg *.bmp *.tga "
                                                 "*.psd *.gif *.hdr *.pic *.pnm"},
                                                false);

                if (selection.size() != 0)
                {
                    // single file is selected
                    auto filePath = selection[0];

                    ref<Texture> texture = Application::s_get().getResourceManager().loadTexture(
                        Texture::Type::DIFFUSE, filePath, false);

                    if (texture)
                    {
                        spriteCmp.p_texture = texture;
                    }
                    else
                    {
                        Log::warn("Could not load texture %s", filePath.c_str());
                    }
                }
            }
            // tooltip for button
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Drag and drop texture files here, or click to browse");
                ImGui::EndTooltip();
            }

            // support drag and drop files onto the image button
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE"))
                {
                    const char*  path = (const char*)payload->Data;
                    ref<Texture> texture
                        = Application::s_get().getResourceManager().loadTexture(Texture::Type::DIFFUSE, path, false);

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
            ImGui::DragFloat("Tiling Factor", &spriteCmp.tilingFactor, 0.01f, 0.0f);

            ImGui::EndTable();

            ImGui::TreePop();
        }
    }

    void _drawTextCmp(Entity& entity)
    {
        static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

        auto& textCmp = entity.getComponent<TextCmp>();

        if (ImGui::TreeNodeEx("Text", flags))
        {
            strncpy_s(m_scratch, textCmp.text.c_str(), textCmp.text.length());

            if (ImGui::InputTextMultiline("##Text", m_scratch, sizeof(m_scratch)))
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
            ImGui::InputText(
                "Font", const_cast<char*>(fontName.c_str()), fontName.length(), ImGuiInputTextFlags_ReadOnly);

            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Drag and drop text files here, or double-click to browse");
                ImGui::EndTooltip();
            }
            // support drag and drop files onto the image button
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE"))
                {
                    const char* filePath = (const char*)payload->Data;
                    ref<Font>   font     = Application::s_get().getResourceManager().loadFont(filePath);

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
                auto selection = util::openFile("Select Font to open", ".", {"Font Files", "*.ttf *.otf"}, false);
                if (selection.size() != 0)
                {
                    // single file is selected
                    auto filePath = selection[0];

                    ref<Font> font = Application::s_get().getResourceManager().loadFont(filePath);

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
                    ImGui::Text("Generating Font Atlas%s", anim[(int)(ImGui::GetTime() / 0.25f) & 3].c_str());
                }
            }

            ImGui::ColorEdit4(
                "Fg Color",
                glm::value_ptr(textCmp.format.fgColor),
                ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);

            ImGui::ColorEdit4(
                "Bg Color",
                glm::value_ptr(textCmp.format.bgColor),
                ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);

            ImGui::DragFloat("Kerning", &textCmp.format.kerning, 0.01f);
            ImGui::DragFloat("Leading", &textCmp.format.leading, 0.01f);

            ImGui::TreePop();
        }
    }

    void _drawParticleEmitterCmp(Entity& entity)
    {
        static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

        auto& pc = entity.getComponent<ParticleEmitterCmp>();

        bool isRuntime = (pc.p_emitter != nullptr);

        if (ImGui::TreeNodeEx("Spawn Options", flags))
        {
            ImGui::Indent();

            ImGui::DragInt("Quantity", (int*)&pc.numParticles, 25.0f, 1, 1000000);

            ImGui::SeparatorText("Spawn Volume");
            const char* spawnTypes[] = {"Point", "Circle", "Rectangle", "Line"};

            ImGui::Combo("Type", (int*)&pc.parameters.spawnVolumeType, spawnTypes, IM_ARRAYSIZE(spawnTypes));

            if (pc.parameters.spawnVolumeType == ParticleEmitter::SpawnVolumeType::POINT)
            {
                // no parameters
            }
            else if (pc.parameters.spawnVolumeType == ParticleEmitter::SpawnVolumeType::CIRCLE)
            {
                ImGui::DragFloat("Radius", &pc.parameters.circleVolumeParams.radius, 0.01);
            }
            else if (pc.parameters.spawnVolumeType == ParticleEmitter::SpawnVolumeType::RECTANGLE)
            {
                ImGui::DragFloat("Width", &pc.parameters.rectVolumeParams.width, 0.01);
                ImGui::DragFloat("Height", &pc.parameters.rectVolumeParams.height, 0.01);
            }
            else if (pc.parameters.spawnVolumeType == ParticleEmitter::SpawnVolumeType::LINE)
            {
                ImGui::DragFloat("Length", &pc.parameters.lineVolumeParams.length, 0.01);
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Relative Position", flags))
        {
            if (_drawVec3Control("Position", pc.parameters.centerPosition, 0.0f, 0.01f))
            {
                if (isRuntime)
                {
                    pc.p_emitter->setPosition(pc.parameters.centerPosition);
                }
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Behavior", flags))
        {
            if (ImGui::Checkbox("Persist", &pc.parameters.persist))
            {
                if (isRuntime)
                {
                    pc.p_emitter->setPersist(pc.parameters.persist);
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Shrink", &pc.parameters.shrink))
            {
                if (isRuntime)
                {
                    pc.p_emitter->setShrink(pc.parameters.shrink);
                }
            }

            if (ImGui::DragFloatRange2("Lifetime",
                                       &pc.parameters.lifetimeMin_s,
                                       &pc.parameters.lifetimeMax_s,
                                       0.05f,
                                       0.0f,
                                       10000.0f,
                                       "Min: %.02f",
                                       "Max: %.02f",
                                       ImGuiSliderFlags_AlwaysClamp))
            {
                if (isRuntime)
                {
                    pc.p_emitter->setLifeTime(pc.parameters.lifetimeMin_s, pc.parameters.lifetimeMax_s);
                }
            }

            bool changedSizeX = false;
            bool changedSizeY = false;

            if (ImGui::DragFloatRange2("Init Size X",
                                       &pc.parameters.initSizeMin.x,
                                       &pc.parameters.initSizeMax.x,
                                       0.001f,
                                       0.0f,
                                       100.0f,
                                       "Min: %.03f",
                                       "Max: %.03f",
                                       ImGuiSliderFlags_AlwaysClamp))
            {
                changedSizeX = true;
            }

            if (ImGui::DragFloatRange2("Init Size Y",
                                       &pc.parameters.initSizeMin.y,
                                       &pc.parameters.initSizeMax.y,
                                       0.001f,
                                       0.0f,
                                       100.0f,
                                       "Min: %.03f",
                                       "Max: %.03f",
                                       ImGuiSliderFlags_AlwaysClamp))
            {
                changedSizeY = true;
            }

            if (isRuntime && (changedSizeX || changedSizeY))
            {
                pc.p_emitter->setInitSize(pc.parameters.initSizeMin, pc.parameters.initSizeMax);
            }

            if (ImGui::DragFloatRange2("Init Speed",
                                       &pc.parameters.initSpeedMin,
                                       &pc.parameters.initSpeedMax,
                                       0.005f,
                                       0.0f,
                                       100.0f,
                                       "Min: %.03f",
                                       "Max: %.03f",
                                       ImGuiSliderFlags_AlwaysClamp))
            {
                if (isRuntime)
                {
                    pc.p_emitter->setInitSpeed(pc.parameters.initSpeedMin, pc.parameters.initSpeedMax);
                }
            }

            ImGui::SeparatorText("Ejection Angle");

            bool changedBase   = false;
            bool changedSpread = false;

            if (ImGui::SliderAngle("Base", &pc.parameters.ejectionBaseAngle_rad))
            {
                changedBase = true;
            }

            if (ImGui::SliderAngle("Spread", &pc.parameters.ejectionSpreadAngle_rad, 0.0f, 360.0f))

            {
                changedSpread = true;
            }

            if (isRuntime && (changedBase || changedSpread))
            {
                pc.p_emitter->setEjectionAngle(pc.parameters.ejectionBaseAngle_rad,
                                               pc.parameters.ejectionSpreadAngle_rad);
            }

            ImGui::SeparatorText("Acceleration");

            bool changedAccelX = false;
            bool changedAccelY = false;
            bool changedAccelZ = false;

            if (ImGui::DragFloatRange2("Accel X",
                                       &pc.parameters.accelerationMin.x,
                                       &pc.parameters.accelerationMax.x,
                                       0.01f,
                                       -100.0f,
                                       100.0f,
                                       "Min: %.02f",
                                       "Max: %.02f",
                                       ImGuiSliderFlags_AlwaysClamp))
            {
                changedAccelX = true;
            }

            if (ImGui::DragFloatRange2("Accel Y",
                                       &pc.parameters.accelerationMin.y,
                                       &pc.parameters.accelerationMax.y,
                                       0.01f,
                                       0.0f,
                                       0.0f,
                                       "Min: %.02f",
                                       "Max: %.02f",
                                       ImGuiSliderFlags_AlwaysClamp))
            {
                changedAccelY = true;
            }

            if (ImGui::DragFloatRange2("Accel Z",
                                       &pc.parameters.accelerationMin.z,
                                       &pc.parameters.accelerationMax.z,
                                       0.01f,
                                       0.0f,
                                       0.0f,
                                       "Min: %.02f",
                                       "Max: %.02f",
                                       ImGuiSliderFlags_AlwaysClamp))
            {
                changedAccelZ = true;
            }

            if (isRuntime && (changedAccelX || changedAccelY || changedAccelZ))
            {
                pc.p_emitter->setAcceleration(pc.parameters.accelerationMin, pc.parameters.accelerationMax);
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Colors", flags))
        {
            const char* blendType[] = {"Additive",
                                       "Subtract",
                                       "Multiply",
                                       "Screen",
                                       "Replace",
                                       "Alpha Blend",
                                       "Alpha Premultiplied",
                                       "Source Alpha"};

            if (ImGui::Combo("Blending", (int*)&pc.parameters.blendingMode, blendType, IM_ARRAYSIZE(blendType)))
            {
                if (isRuntime)
                {
                    pc.p_emitter->setBlendMode(pc.parameters.blendingMode);
                }
            }

            for (uint32_t i = 0; i < pc.parameters.colors.size(); i++)
            {
                auto& colorSpec = pc.parameters.colors[i];

                bool startChanged = false;
                bool endChanged   = false;

                std::string startNm = "Start " + std::to_string(i);
                std::string endNm   = "End " + std::to_string(i);

                if (ImGui::ColorEdit4(
                        startNm.c_str(),
                        glm::value_ptr(colorSpec.colorStart),
                        ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview))
                {
                    startChanged = true;
                }
                if (ImGui::ColorEdit4(
                        endNm.c_str(),
                        glm::value_ptr(colorSpec.colorEnd),
                        ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview))
                {
                    endChanged = true;
                }

                if (isRuntime && (startChanged || endChanged))
                {
                    pc.p_emitter->setColor(i, colorSpec);
                }
            }

            if (ImGui::Button("Add Color"))
            {
                pc.parameters.colors.push_back({glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)});

                if (isRuntime)
                {
                    pc.p_emitter->addColor({glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)});

                    pc.p_emitter->chooseColors(0, pc.parameters.colors.size() - 1);
                }
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Texture", flags))
        {
            // Draw the loaded texture as a button image
            void* textureId = nullptr;
            if (pc.p_texture == nullptr)
            {
                textureId = reinterpret_cast<void*>(mp_checkerboardTex->getId());
            }
            else
            {
                textureId = reinterpret_cast<void*>(pc.p_texture->getId());
            }

            ImGui::BeginTable("Textures", 2, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoHostExtendX);
            ImGui::TableSetupColumn("TexImage", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Options", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (ImGui::ImageButton(textureId, {50, 50}))
            {
                // open the file dialog
                auto selection = util::openFile("Select Texture to open",
                                                ".",
                                                {"Image Files",
                                                 "*.png *.jpg *.jpeg *.bmp *.tga "
                                                 "*.psd *.gif *.hdr *.pic *.pnm"},
                                                false);

                if (selection.size() != 0)
                {
                    // single file is selected
                    auto filePath = selection[0];

                    ref<Texture> texture = Application::s_get().getResourceManager().loadTexture(
                        Texture::Type::DIFFUSE, filePath, false);

                    if (texture)
                    {
                        pc.p_texture = texture;
                    }
                    else
                    {
                        Log::warn("Could not load texture %s", filePath.c_str());
                    }
                }
            }
            // tooltip for button
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Drag and drop texture files here, or click to browse");
                ImGui::EndTooltip();
            }

            // support drag and drop files onto the image button
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE"))
                {
                    const char*  path = (const char*)payload->Data;
                    ref<Texture> texture
                        = Application::s_get().getResourceManager().loadTexture(Texture::Type::DIFFUSE, path, false);

                    if (texture)
                    {
                        pc.p_texture = texture;
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

            ImGui::EndTable();

            ImGui::TreePop();
        }
    }

    void _drawCameraCmp(Entity& entity)
    {
        auto& cameraCmp = entity.getComponent<CameraCmp>();

        const char* cameraTypes[] = {"Orthographic", "Perspective"};

        int currentType = static_cast<int>(cameraCmp.camera.getType());
        if (ImGui::Combo("Type", &currentType, cameraTypes, IM_ARRAYSIZE(cameraTypes)))
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

        if (_drawVec3Control("Position", position, 0.0f, 0.01f))
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
        auto& tc = entity.getComponent<TransformCmp>();

        glm::vec3 translation = tc.local.getTranslation();

        if (_drawVec3Control("Translation", translation, 0.0f, 0.01f))
        {
            tc.local.setTranslation(translation);
        }

        glm::vec3 rotation = glm::degrees(tc.local.getRotation());

        if (_drawVec3Control("Rotation", rotation, 0.0f, 0.1f))
        {
            tc.local.setRotation(glm::radians(rotation));
        }

        glm::vec3 scale = tc.local.getScale();

        bool locked = tc.local.isScaleLocked();
        if (_drawVec3Control("Scale", scale, 1.0f, 0.01f, true, &locked))
        {
            tc.local.setScaleLocked(locked);

            if (tc.local.isScaleLocked())
            {
                // check what changed
                if (scale.x != tc.local.getScale().x)
                {
                    tc.local.setScaleX(scale.x);
                }
                else if (scale.y != tc.local.getScale().y)
                {
                    tc.local.setScaleY(scale.y);
                }
                else if (scale.z != tc.local.getScale().z)
                {
                    tc.local.setScaleZ(scale.z);
                }
            }
            else
            {
                tc.local.setScale(scale);
            }
        }
    }

    void _drawComponents(Entity& entity)
    {
        static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen
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

            if (_drawAddComponentEntry<ParticleEmitterCmp>(ICON_FA_EXPLOSION " Particle Emitter"))
            {
                if (!entity.hasComponent<TransformCmp>())
                {
                    entity.addComponent<TransformCmp>();
                }
            }

            _drawAddComponentEntry<NativeLogicCmp>(ICON_FA_COMPUTER " Native Logic");
            _drawAddComponentEntry<CameraCmp>(ICON_FA_VIDEO " Camera");

            ImGui::EndPopup();
        }

        ImGui::Separator();
        ImGui::BeginChild("##Components");

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
        // Particle Emitter Component
        ///////////////////////////
        if (entity.hasComponent<ParticleEmitterCmp>())
        {
            bool open    = ImGui::TreeNodeEx(ICON_FA_EXPLOSION " Particle Emitter", flags);
            bool removed = _drawComponentMenu();

            if (open)
            {
                _drawParticleEmitterCmp(entity);
                ImGui::TreePop();
            }

            if (removed)
            {
                entity.removeComponent<ParticleEmitterCmp>();
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

        ImGui::EndChild();
    }

    // custom stylized 3 input (X,Y,Z) control block
    static bool _drawVec3Control(const std::string& label,
                                 glm::vec3&         values,
                                 float              resetValue,
                                 float              speed,
                                 bool               lockable     = false,
                                 bool*              lockedStatus = nullptr)
    {
        // set the size of the dragfloats
        static float itemWidth = ImGui::CalcTextSize("A").x * 6.0;

        static float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;

        static ImVec2 buttonSize = {lineHeight - 3, lineHeight};

        ImGuiIO& io       = ImGui::GetIO();
        auto     boldfont = io.Fonts->Fonts[1];

        // set the size of the buttons
        bool changedX = false;
        bool changedY = false;
        bool changedZ = false;

        ImGui::BeginTable("table", 2, ImGuiTableFlags_SizingFixedFit);
        ImGui::TableSetupColumn("Data", ImGuiTableColumnFlags_WidthFixed, (itemWidth * 3 + buttonSize.x * 3));
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});

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
        bool changed = ImGui::DragFloat("##X", &values.x, speed, 0.0f, 0.0f, "%.2f");
        if (changed)
            changedX = true;

        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});

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
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});

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

            if (ImGui::Button(*lockedStatus ? ICON_FA_LOCK : ICON_FA_LOCK_OPEN))
            {
                *lockedStatus = !*lockedStatus;
                changedLock   = true;
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