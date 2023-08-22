#include "nimbus.hpp"

//////////////////////////////////////////////////////
// ENTRY POINT
//////////////////////////////////////////////////////
#include "nimbus/core/entry.hpp"
//////////////////////////////////////////////////////

#include "panels/viewportPanel.hpp"
#include "panels/sceneControlPanel.hpp"
#include "panels/sceneHierarchyPanel.hpp"
#include "panels/renderStatsPanel.hpp"

#include <filesystem>

// for some reason when looking straight on, the scale gizmo is broken
// so shift the camera slightly off origin until this is fixed.
#define IMGUIZO_SCALE_FIXED 0
#include "panels/editCameraMenuPanel.hpp"

namespace nimbus
{
class sceneCameraController : public EntityLogic
{
   public:
    virtual void onCreate()
    {
        mp_cameraCmp = &getComponent<CameraCmp>();
        mp_window    = getComponent<WindowRefCmp>().p_window;

        m_lastWheelPos = mp_window->mouseWheelPos();

        mp_cameraCmp->camera.setPosition({0.0f, 0.0f, 0.0f});
        mp_cameraCmp->camera.setYaw(0.0f);
        mp_cameraCmp->camera.setPitch(0.0f);
        mp_cameraCmp->camera.setZoom(1.0f);
    }

    virtual void onDestroy()
    {
    }

    virtual void onUpdate(f32_t deltaTime)
    {
        // don't update this camera if we aren't using it
        if (!mp_cameraCmp->primary)
        {
            return;
        }

        if (mp_window->keyPressed(ScanCode::W))
        {
            mp_cameraCmp->camera.processPosiUpdate(Camera::Movement::UP, deltaTime);
        }
        if (mp_window->keyPressed(ScanCode::A))
        {
            mp_cameraCmp->camera.processPosiUpdate(Camera::Movement::BACKWARD, deltaTime);
        }
        if (mp_window->keyPressed(ScanCode::S))
        {
            mp_cameraCmp->camera.processPosiUpdate(Camera::Movement::DOWN, deltaTime);
        }
        if (mp_window->keyPressed(ScanCode::D))
        {
            mp_cameraCmp->camera.processPosiUpdate(Camera::Movement::FORWARD, deltaTime);
        }

        f32_t curWheelPos = mp_window->mouseWheelPos();

        if (curWheelPos != m_lastWheelPos)
        {
            mp_cameraCmp->camera.processZoom((curWheelPos - m_lastWheelPos) * 0.5);
            m_lastWheelPos = curWheelPos;
        }
    }

   private:
    CameraCmp* mp_cameraCmp;
    Window*    mp_window;
    f32_t      m_lastWheelPos;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main application implementation is in this layer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FelixLayer : public Layer
{
   public:
    enum class State
    {
        STOP,
        PLAY
    };

    ///////////////////////////
    // References
    ///////////////////////////
    Application* mp_appRef;
    Window*      mp_appWinRef;

    ///////////////////////////
    // Scene
    ///////////////////////////
    ref<Scene>  mp_scene;
    State       m_sceneState      = State::STOP;
    std::string m_openedScenePath = "";
    Entity      m_selectedEntity  = {};

    ///////////////////////////
    // Viewport Info
    ///////////////////////////
    glm::vec2 m_viewportSize = {800, 600};

    f32_t m_aspectRatio     = 800 / 600;
    bool  m_viewportFocused = false;
    bool  m_viewportHovered = false;

    ref<Font> mp_generalFont = nullptr;

    ///////////////////////////
    // Framebuffers
    ///////////////////////////
    ref<Framebuffer> mp_frameBuffer;
    ref<Framebuffer> mp_screenBuffer;

    ///////////////////////////
    // Cameras
    ///////////////////////////
    ref<Camera> mp_editCamera;

    ///////////////////////////
    // Panels
    ///////////////////////////
    scope<ViewportPanel>       mp_viewportPanel;
    scope<SceneControlPanel>   mp_sceneControlPanel;
    scope<SceneHeirarchyPanel> mp_sceneHierarchyPanel;
    scope<RenderStatsPanel>    mp_renderStatsPanel;
    scope<EditCameraMenuPanel> mp_editCameraMenuPanel;

    ///////////////////////////
    // Flags for GUI events
    ///////////////////////////
    bool        m_fileDropHandled = true;
    std::string m_fileDropPath;

    FelixLayer() : Layer(Layer::Type::REGULAR, "Felix")
    {
    }

    ~FelixLayer()
    {
    }

    virtual void onInsert() override
    {
        Log::info("Hello from Felix!");

        mp_appRef    = &Application::s_get();
        mp_appWinRef = &mp_appRef->getWindow();
        mp_appRef->setDrawPeriodLimit(0.000f);

        mp_scene = ref<Scene>::gen("Demo Scene");

        ///////////////////////////
        // Edit Camera
        ///////////////////////////
        mp_editCamera = ref<Camera>::gen(Camera::Type::PERSPECTIVE);
        mp_editCamera->setAspectRatio(m_aspectRatio);
#if IMGUIZO_SCALE_FIXED
        mp_editCamera->setPosition({0.00f, 0.00f, 10 * 2.4125f});
#else
        mp_editCamera->setPosition({0.01f, -0.01f, 10 * 2.4125f});
#endif
        mp_editCamera->setYaw(-90.0f);
        mp_editCamera->setSpeed(5.0f);

        ///////////////////////////
        // Panels
        ///////////////////////////
        mp_viewportPanel       = genScope<ViewportPanel>(mp_editCamera.raw(), mp_scene);
        mp_sceneControlPanel   = genScope<SceneControlPanel>();
        mp_sceneHierarchyPanel = genScope<SceneHeirarchyPanel>(mp_scene);
        mp_renderStatsPanel    = genScope<RenderStatsPanel>();
        mp_editCameraMenuPanel = genScope<EditCameraMenuPanel>(mp_editCamera.raw());

        mp_viewportPanel->setEntitySelectedCallback(
            std::bind(&FelixLayer::_onEntitySelected, this, std::placeholders::_1));

        mp_sceneHierarchyPanel->setEntitySelectedCallback(
            std::bind(&FelixLayer::_onEntitySelected, this, std::placeholders::_1));

        ///////////////////////////
        // Test Camera
        ///////////////////////////
        // auto sceneCameraEntity = mp_scene->addEntity("Scene Camera");
        // sceneCameraEntity.addComponent<CameraCmp>();

        // sceneCameraEntity.addComponent<WindowRefCmp>(mp_appWinRef);

        // sceneCameraEntity.addComponent<NativeLogicCmp>()
        //     .bind<sceneCameraController>();

        // ///////////////////////////
        // // Test Sprite
        // ///////////////////////////
        // u32_t sz = 50;
        // for (u32_t i = 0; i < sz; i++)
        // {
        //     for (u32_t j = 0; j < sz; j++)
        //     {
        //         auto  spriteEntity1 = mp_scene->addEntity("Test Sprite " + std::to_string((i * sz) + j));
        //         auto& transformCmp1 = spriteEntity1.addComponent<TransformCmp>();
        //         transformCmp1.local.setScale({0.1f, 0.1f, 1.0f});
        //         transformCmp1.local.setTranslationX(0.13 * i);
        //         transformCmp1.local.setTranslationY(0.13 * j);
        //         transformCmp1.local.setRotationZ(glm::radians(45.0f));
        //         auto& spriteCmp1 = spriteEntity1.addComponent<SpriteCmp>();
        //         spriteCmp1.color = {0.0f, 0.02f * i, 0.02f * j, 1.0f};
        //     }
        // }

        // auto  spriteEntity2 = mp_scene->addEntity("Test Sprite 2");
        // auto& transformCmp2 = spriteEntity2.addComponent<TransformCmp>();
        // transformCmp2.local.setScale({0.75f, 0.75f, 1.0f});
        // auto& spriteCmp2 = spriteEntity2.addComponent<SpriteCmp>();
        // spriteCmp2.color = {0.1f, 0.9f, 0.2f, 1.0f};

        ///////////////////////////
        // Text text
        ///////////////////////////
        // mp_generalFont
        //     = Application::s_get().getResourceManager().loadFont("../resources/fonts/Roboto/Roboto-Regular.ttf");

        // Font::Format format;
        // format.p_font  = mp_generalFont;
        // format.fgColor = {0.0f, 0.5f, 0.7f, 1.0f};
        // format.bgColor = {0.0f, 0.0f, 0.0f, 0.0f};
        // format.kerning = 0.0f;

        // auto  textEntity    = mp_scene->addEntity("Test Text");
        // auto& transformCmp3 = textEntity.addComponent<TransformCmp>();
        // transformCmp3.local.setScale({0.25f, 0.25f, 1.0f});
        // transformCmp3.local.setTranslationX(-0.4f);
        // transformCmp3.local.setTranslationY(0.30f);
        // auto textCmp = textEntity.addComponent<TextCmp>("Yuge File", format);

        // mp_scene->sortEntities();

        ///////////////////////////
        // Setup Framebuffers
        ///////////////////////////
        // multisampled buffer we render to
        Framebuffer::Spec fbSpec;
        fbSpec.width   = m_viewportSize.x;
        fbSpec.height  = m_viewportSize.y;
        fbSpec.samples = 4;

        Texture::Spec fbMainTex;
        fbMainTex.width          = fbSpec.width;
        fbMainTex.height         = fbSpec.height;
        fbMainTex.samples        = fbSpec.samples;
        fbMainTex.format         = Texture::Format::RGBA;
        fbMainTex.formatInternal = Texture::FormatInternal::RGBA8;
        fbMainTex.dataType       = Texture::DataType::UNSIGNED_BYTE;
        fbMainTex.filterTypeMin  = Texture::FilterType::LINEAR;
        fbMainTex.filterTypeMag  = Texture::FilterType::LINEAR;
        fbMainTex.wrapTypeR      = Texture::WrapType::CLAMP_TO_EDGE;
        fbMainTex.wrapTypeS      = Texture::WrapType::CLAMP_TO_EDGE;
        fbMainTex.wrapTypeT      = Texture::WrapType::CLAMP_TO_EDGE;
        fbMainTex.clearColor     = std::array<f32_t, 4>{0.1f, 0.1f, 0.1f, 1.0f};

        fbSpec.colorAttachments.push_back(fbMainTex);

        Texture::Spec fbEntIdTexSpec;
        fbEntIdTexSpec.width          = fbSpec.width;
        fbEntIdTexSpec.height         = fbSpec.height;
        fbEntIdTexSpec.samples        = fbSpec.samples;
        fbEntIdTexSpec.format         = Texture::Format::RED_INT;
        fbEntIdTexSpec.formatInternal = Texture::FormatInternal::R32UI;
        fbEntIdTexSpec.dataType       = Texture::DataType::UNSIGNED_INT;
        fbEntIdTexSpec.filterTypeMin  = Texture::FilterType::NEAREST;
        fbEntIdTexSpec.filterTypeMag  = Texture::FilterType::NEAREST;
        fbEntIdTexSpec.wrapTypeR      = Texture::WrapType::CLAMP_TO_EDGE;
        fbEntIdTexSpec.wrapTypeS      = Texture::WrapType::CLAMP_TO_EDGE;
        fbEntIdTexSpec.wrapTypeT      = Texture::WrapType::CLAMP_TO_EDGE;
        fbEntIdTexSpec.clearColor     = std::array<u32_t, 4>{Entity::k_nullEntity};

        fbSpec.colorAttachments.push_back(fbEntIdTexSpec);

        fbSpec.depthType = Texture::FormatInternal::DEPTH24_STENCIL8;

        mp_frameBuffer = Framebuffer::s_create(fbSpec);

        // buffer we blit multisampled buffer to for output to screen
        Framebuffer::Spec screenSpec;
        screenSpec.width   = m_viewportSize.x;
        screenSpec.height  = m_viewportSize.y;
        screenSpec.samples = 1;
        Texture::Spec sbMainTex;
        sbMainTex.width          = screenSpec.width;
        sbMainTex.height         = screenSpec.height;
        sbMainTex.samples        = screenSpec.samples;
        sbMainTex.format         = Texture::Format::RGBA;
        sbMainTex.formatInternal = Texture::FormatInternal::RGBA8;
        sbMainTex.dataType       = Texture::DataType::UNSIGNED_BYTE;
        sbMainTex.filterTypeMin  = Texture::FilterType::LINEAR;
        sbMainTex.filterTypeMag  = Texture::FilterType::LINEAR;
        sbMainTex.wrapTypeR      = Texture::WrapType::CLAMP_TO_EDGE;
        sbMainTex.wrapTypeS      = Texture::WrapType::CLAMP_TO_EDGE;
        sbMainTex.wrapTypeT      = Texture::WrapType::CLAMP_TO_EDGE;
        sbMainTex.clearColor     = std::array<f32_t, 4>{0.1f, 0.1f, 0.1f, 1.0f};

        screenSpec.colorAttachments.push_back(sbMainTex);

        Texture::Spec sbEntIdTexSpec;
        sbEntIdTexSpec.width          = screenSpec.width;
        sbEntIdTexSpec.height         = screenSpec.height;
        sbEntIdTexSpec.samples        = screenSpec.samples;
        sbEntIdTexSpec.format         = Texture::Format::RED_INT;
        sbEntIdTexSpec.formatInternal = Texture::FormatInternal::R32UI;
        sbEntIdTexSpec.dataType       = Texture::DataType::UNSIGNED_INT;
        sbEntIdTexSpec.filterTypeMin  = Texture::FilterType::NEAREST;
        sbEntIdTexSpec.filterTypeMag  = Texture::FilterType::NEAREST;
        sbEntIdTexSpec.wrapTypeR      = Texture::WrapType::CLAMP_TO_EDGE;
        sbEntIdTexSpec.wrapTypeS      = Texture::WrapType::CLAMP_TO_EDGE;
        sbEntIdTexSpec.wrapTypeT      = Texture::WrapType::CLAMP_TO_EDGE;
        sbEntIdTexSpec.clearColor     = std::array<u32_t, 4>{Entity::k_nullEntity};

        screenSpec.colorAttachments.push_back(sbEntIdTexSpec);

        screenSpec.depthType = Texture::FormatInternal::NONE;

        mp_screenBuffer = Framebuffer::s_create(screenSpec);
    }

    virtual void onRemove() override
    {
    }

    virtual void onUpdate(f32_t deltaTime) override
    {
        ///////////////////////////
        // Start/Stop Scene
        ///////////////////////////
        if (mp_sceneControlPanel->getState().runState == SceneControlPanel::RunState::PLAY
            && m_sceneState != State::PLAY)
        {
            m_sceneState = State::PLAY;
            mp_scene->onStartRuntime();
        }
        else if (mp_sceneControlPanel->getState().runState == SceneControlPanel::RunState::STOP
                 && m_sceneState != State::STOP)
        {
            m_sceneState = State::STOP;
            mp_scene->onStopRuntime();
        }

        ///////////////////////////
        // Update panels
        ///////////////////////////
        mp_viewportPanel->onUpdate(m_selectedEntity);

        ///////////////////////////
        // Update scene
        ///////////////////////////
        if (m_sceneState == State::PLAY)
        {
            mp_scene->onUpdateRuntime(deltaTime);
        }
        else if (m_sceneState == State::STOP)
        {
            if (m_viewportHovered)
            {
                _handleKeyboardInput(deltaTime);
            }

            mp_scene->_onUpdateEditor(deltaTime);
        }
    }

    virtual void onDraw(f32_t deltaTime) override
    {
        mp_frameBuffer->bind();
        mp_frameBuffer->clearAllAttachments();

        if (mp_renderStatsPanel->m_wireFrame != GraphicsApi::getWireframe())
        {
            GraphicsApi::setWireframe(mp_renderStatsPanel->m_wireFrame);
        }

        Renderer2D::s_resetStats();

        if (m_sceneState == State::STOP)
        {
            mp_scene->_onDrawEditor(mp_editCamera.raw());
        }
        else
        {
            mp_scene->onDrawRuntime();
        }

        mp_frameBuffer->blit(mp_screenBuffer, 0, 0);
        mp_frameBuffer->blit(mp_screenBuffer, 1, 1);

        mp_frameBuffer->unbind();

        NB_UNUSED(deltaTime);
    }

    void _open()
    {
        auto selection = util::openFile("Select Scene to open", ".", {"Scene Files", "*.nmscn"}, false);

        if (selection.size() != 0)
        {
            auto filePath = selection[0];

            // stop old scene
            if (m_sceneState == State::PLAY)
            {
                mp_scene->onStopRuntime();
                mp_sceneControlPanel->setRunState(SceneControlPanel::RunState::STOP);
            }

            // throw away old scene (should probably be in a new scene func)
            mp_scene = ref<Scene>::gen("Loading scene");
            mp_sceneHierarchyPanel->setSceneContext(mp_scene);
            mp_viewportPanel->setSceneContext(mp_scene);
            m_selectedEntity = {};

            SceneSerializer ss = SceneSerializer(mp_scene);
            ss.deserialize(filePath);

            m_openedScenePath = filePath;
        }
    }

    void _save(bool as)
    {
        std::string path;

        if (as || m_openedScenePath.empty())
        {
            auto selection = util::saveFile("Save scene as", ".", {"Scene Files", "*.nmscn"});

            if (!selection.empty())
            {
                std::filesystem::path fPath(selection);

                fPath.replace_extension(".nmscn");

                path = fPath.generic_string();

                m_openedScenePath = path;
            }
        }
        else
        {
            path = m_openedScenePath;
        }

        SceneSerializer ss = SceneSerializer(mp_scene);
        ss.serialize(path);
    }

    virtual void onEvent(Event& event) override
    {
        Event::Type eventType = event.getEventType();

        // We only care about these events if we're paused
        if (m_sceneState == State::STOP)
        {
            if (eventType == Event::Type::MOUSEMOTION)
            {
                if (mp_appWinRef->mouseButtonPressed(MouseButton::MIDDLE)
                    && mp_editCamera->getType() == Camera::Type::PERSPECTIVE)
                {
                    glm::vec2 delta = {(f32_t)event.getDetails().motion.xrel, (f32_t)event.getDetails().motion.yrel};

                    static const f32_t orbitScale = 4.0f;
                    mp_editCamera->processViewUpdate(delta * orbitScale, true);
                }

                event.markAsHandled();
            }
            else if (eventType == Event::Type::MOUSEWHEEL)
            {
                f32_t zoomAmount = event.getDetails().wheel.preciseY;

                mp_editCamera->processZoom(zoomAmount);

                event.markAsHandled();
            }

            else if (eventType == Event::Type::DROPFILE)
            {
                m_fileDropHandled = false;
                m_fileDropPath    = event.getDetails().drop.file;
                event.markAsHandled();
            }

            // shortcuts TODO make this less hacky
            else if (eventType == Event::Type::KEYDOWN)
            {
                if (mp_appWinRef->modKeyPressed(KeyMod::CTRL))
                {
                    // open
                    if (mp_appWinRef->keyPressed(ScanCode::O))
                    {
                        _open();
                    }
                    // save, saveas
                    else if (mp_appWinRef->keyPressed(ScanCode::S))
                    {
                        if (mp_appWinRef->modKeyPressed(KeyMod::SHIFT))
                        {
                            _save(true);
                        }
                        else
                        {
                            _save(false);
                        }
                    }
                }
                else
                {
                    if (mp_appWinRef->keyPressed(ScanCode::ESCAPE))
                    {
                        if (m_selectedEntity)
                        {
                            if (mp_sceneControlPanel->getState().toolState != SceneControlPanel::ToolState::NONE)
                            {
                                mp_sceneControlPanel->getState().toolState = SceneControlPanel::ToolState::NONE;
                            }
                        }
                    }
                    if (mp_appWinRef->keyPressed(ScanCode::U))
                    {
                        if (m_selectedEntity)
                        {
                            mp_sceneControlPanel->getState().toolState = SceneControlPanel::ToolState::UNIVERSAL;
                        }
                    }
                    else if (mp_appWinRef->keyPressed(ScanCode::M))
                    {
                        if (m_selectedEntity)
                        {
                            mp_sceneControlPanel->getState().toolState = SceneControlPanel::ToolState::MOVE;
                        }
                    }
                    else if (mp_appWinRef->keyPressed(ScanCode::R))
                    {
                        if (m_selectedEntity)
                        {
                            mp_sceneControlPanel->getState().toolState = SceneControlPanel::ToolState::ROTATE;
                        }
                    }
                    else if (mp_appWinRef->keyPressed(ScanCode::E))
                    {
                        if (m_selectedEntity)
                        {
                            mp_sceneControlPanel->getState().toolState = SceneControlPanel::ToolState::SCALE;
                        }
                    }
                }
            }
        }
    }

    void _handleKeyboardInput(f32_t deltaTime)
    {
        ///////////////////////////
        // Camera Controls
        ///////////////////////////

        if (mp_editCamera->getType() == Camera::Type::PERSPECTIVE)
        {
            if (mp_appWinRef->keyPressed(ScanCode::W))
            {
                mp_editCamera->processPosiUpdate(Camera::Movement::FORWARD, deltaTime);
            }
            if (mp_appWinRef->keyPressed(ScanCode::S))
            {
                mp_editCamera->processPosiUpdate(Camera::Movement::BACKWARD, deltaTime);
            }
            if (mp_appWinRef->keyPressed(ScanCode::SPACE))
            {
                mp_editCamera->processPosiUpdate(Camera::Movement::UP, deltaTime);
            }
            if (mp_appWinRef->keyPressed(ScanCode::C))
            {
                mp_editCamera->processPosiUpdate(Camera::Movement::DOWN, deltaTime);
            }
            if (mp_appWinRef->keyPressed(ScanCode::A))
            {
                mp_editCamera->processPosiUpdate(Camera::Movement::LEFT, deltaTime);
            }
            if (mp_appWinRef->keyPressed(ScanCode::D))
            {
                mp_editCamera->processPosiUpdate(Camera::Movement::RIGHT, deltaTime);
            }
        }
        else
        {
            if (mp_appWinRef->keyPressed(ScanCode::W))
            {
                mp_editCamera->processPosiUpdate(Camera::Movement::UP, deltaTime);
            }
            if (mp_appWinRef->keyPressed(ScanCode::A))
            {
                mp_editCamera->processPosiUpdate(Camera::Movement::BACKWARD, deltaTime);
            }
            if (mp_appWinRef->keyPressed(ScanCode::S))
            {
                mp_editCamera->processPosiUpdate(Camera::Movement::DOWN, deltaTime);
            }
            if (mp_appWinRef->keyPressed(ScanCode::D))
            {
                mp_editCamera->processPosiUpdate(Camera::Movement::FORWARD, deltaTime);
            }
        }
    }

    void _makeDockspace()
    {
        static bool               dockspaceOpen           = true;
        static bool               optFullscreenPersistant = true;
        bool                      optFullscreen           = optFullscreenPersistant;
        static ImGuiDockNodeFlags dockspaceFlags          = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent
        // window not dockable into, because it would be confusing to have two
        // docking targets within each others.
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (optFullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
                           | ImGuiWindowFlags_NoMove;
            windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will
        // render our background and handle the pass-thru hole, so we ask
        // Begin() to not render a background.
        if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
            windowFlags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka
        // window is collapsed). This is because we want to keep our DockSpace()
        // active. If a DockSpace() is inactive, all active windows docked into
        // it will lose their parent and become undocked. We cannot preserve the
        // docking relationship between an active window and an inactive
        // docking, otherwise any change of dockspace/settings would lead to
        // windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", &dockspaceOpen, windowFlags);
        ImGui::PopStyleVar();
        if (optFullscreen)
        {
            ImGui::PopStyleVar(2);
        }

        ImGuiIO&    io          = ImGui::GetIO();
        ImGuiStyle& style       = ImGui::GetStyle();
        f32_t       minWinSizeX = style.WindowMinSize.x;
        style.WindowMinSize.x   = 325.0f;
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspaceId = ImGui::GetID("FelixDockSpace");
            ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);
        }
        style.WindowMinSize.x = minWinSizeX;
    }

    virtual void onGuiDraw(f32_t deltaTime) override
    {
        ///////////////////////////
        // Allow external file drops
        ///////////////////////////
        if (!m_fileDropHandled)
        {
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceExtern))
            {
                // For simplicity, handle only one file drop at a time.
                ImGui::SetDragDropPayload("DND_FILE", m_fileDropPath.c_str(), m_fileDropPath.length() + 1);

                ImGui::EndDragDropSource();
            }
            m_fileDropHandled = true;
        }

        ///////////////////////////
        // Dockspace
        ///////////////////////////
        _makeDockspace();

        ///////////////////////////
        // Menu
        ///////////////////////////
        if (ImGui::BeginMainMenuBar())
        {
            ImGui::TextUnformatted(ICON_FA_CAT " Felix");
            ImGui::SameLine();

            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New"))
                {
                    // Handle when 'New' is clicked
                }

                if (ImGui::MenuItem("Open", "Ctrl+O"))
                {
                    _open();
                }

                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {
                    _save(false);
                }

                if (ImGui::MenuItem("Save-as", "Ctrl+Shift+S"))
                {
                    _save(true);
                }

                if (ImGui::MenuItem("Test call script"))
                {
                    ScriptEngine::s_testCallScript();
                }

                if (ImGui::MenuItem("Unload Script Assembly"))
                {
                    ScriptEngine::s_unloadScriptAssembly();
                }

                if (ImGui::MenuItem("Load Script Assembly"))
                {
                    ScriptEngine::s_loadScriptAssembly();
                }

                if (ImGui::MenuItem("Exit"))
                {
                    // Handle when 'Exit' is clicked
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "Ctrl+Z"))
                {
                    // Handle when 'Undo' is clicked
                }

                if (ImGui::MenuItem("Redo", "Ctrl+Y"))
                {
                    // Handle when 'Redo' is clicked
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        // Bounds used for some calculations
        Camera::Bounds worldBounds;
        if (mp_editCamera->getType() == Camera::Type::ORTHOGRAPHIC)
        {
            worldBounds = mp_editCamera->getVisibleWorldBounds();
        }

        ///////////////////////////
        // Scene Control
        ///////////////////////////
        mp_sceneControlPanel->onDraw();

        ///////////////////////////
        // Viewport
        ///////////////////////////
        mp_viewportPanel->onDraw(mp_screenBuffer,
                                 mp_editCamera->getType() == Camera::Type::ORTHOGRAPHIC,
                                 worldBounds,
                                 m_selectedEntity,
                                 mp_sceneControlPanel->getState().toolState);

        m_viewportFocused = mp_viewportPanel->m_viewportFocused;
        m_viewportHovered = mp_viewportPanel->m_viewportHovered;

        ///////////////////////////
        // Scene Heirarchy
        ///////////////////////////
        mp_sceneHierarchyPanel->onDraw(m_selectedEntity);

        ///////////////////////////
        // Camera Menu
        ///////////////////////////
        mp_editCameraMenuPanel->onDraw(worldBounds);

        ///////////////////////////
        // Render Stats
        ///////////////////////////
        mp_renderStatsPanel->onDraw(deltaTime);

        ImGui::End();  // dockspace

        if (mp_viewportPanel->wasResized())
        {
            // we need to resize some stuff
            m_viewportSize = mp_viewportPanel->m_viewportSize;
            m_aspectRatio  = m_viewportSize.x / m_viewportSize.y;
            mp_editCamera->setAspectRatio(m_aspectRatio);

            mp_frameBuffer->resize(m_viewportSize.x, m_viewportSize.y);
            mp_screenBuffer->resize(m_viewportSize.x, m_viewportSize.y);

            mp_scene->onResize(m_viewportSize.x, m_viewportSize.y);
        }
    }

    void _onEntitySelected(Entity entity)
    {
        m_selectedEntity = entity;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Make the application and start it
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Felix : public Application
{
   public:
    Felix() : Application("Felix", 1920, 1080)
    {
        Application::insertLayer(ref<FelixLayer>::gen());
    }
};

////////////////////////////////////////
/// Entry point
///////////////////////////////////////
Application* createApplication()
{
    return new Felix();
}

}  // namespace nimbus