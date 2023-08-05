#include "nimbus.hpp"
#include "nimbus/core/entry.hpp"
#include "panels/viewportPanel.hpp"
#include "panels/sceneControlPanel.hpp"
#include "panels/sceneHierarchyPanel.hpp"
#include "panels/renderStatsPanel.hpp"
#include "panels/editCameraMenuPanel.hpp"


namespace nimbus
{
class sceneCameraController : public EntityLogic
{
   public:
    virtual void onCreate()
    {
        mp_cameraCmp = &getComponent<CameraCmp>();
        mp_window = getComponent<WindowRefCmp>().p_window;

        m_lastWheelPos = mp_window->mouseWheelPos();

        mp_cameraCmp->p_camera->setPosition({0.0f, 0.0f, 0.0f});
        mp_cameraCmp->p_camera->setYaw(0.0f);
        mp_cameraCmp->p_camera->setPitch(0.0f);
        mp_cameraCmp->p_camera->setZoom(1.0f);
    }

    virtual void onDestroy()
    {
    }

    virtual void onUpdate(float deltaTime)
    {
        // don't update this camera if we aren't using it
        if (!mp_cameraCmp->primary)
        {
            return;
        }

        if (mp_window->keyPressed(ScanCode::W))
        {
            mp_cameraCmp->p_camera->processPosiUpdate(Camera::Movement::UP,
                                                      deltaTime);
        }
        if (mp_window->keyPressed(ScanCode::A))
        {
            mp_cameraCmp->p_camera->processPosiUpdate(
                Camera::Movement::BACKWARD, deltaTime);
        }
        if (mp_window->keyPressed(ScanCode::S))
        {
            mp_cameraCmp->p_camera->processPosiUpdate(Camera::Movement::DOWN,
                                                      deltaTime);
        }
        if (mp_window->keyPressed(ScanCode::D))
        {
            mp_cameraCmp->p_camera->processPosiUpdate(Camera::Movement::FORWARD,
                                                      deltaTime);
        }

        float curWheelPos = mp_window->mouseWheelPos();

        if (curWheelPos != m_lastWheelPos)
        {
            mp_cameraCmp->p_camera->processZoom((curWheelPos - m_lastWheelPos)
                                                * 0.5);
            m_lastWheelPos = curWheelPos;
        }
    }

   private:
    CameraCmp* mp_cameraCmp;
    Window*    mp_window;
    float      m_lastWheelPos;
};

////////////////////////////////////////////////////////////////////////////////
// Main application implementation is in this layer
////////////////////////////////////////////////////////////////////////////////
class FelixLayer : public Layer
{
   public:
    enum class State
    {
        PAUSE,
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
    ref<Scene>   mp_scene;
    State        m_sceneState = State::PAUSE;


    ///////////////////////////
    // Viewport Info
    ///////////////////////////
    glm::vec2 m_viewportSize    = {800, 600};
    float     m_aspectRatio     = 800 / 600;
    bool      m_viewportFocused = false;
    bool      m_viewportHovered = false;

    ref<Font> mp_generalFont = nullptr;

    ///////////////////////////
    // Framebuffers
    ///////////////////////////
    ref<FrameBuffer> mp_frameBuffer;
    ref<FrameBuffer> mp_screenBuffer;

    ///////////////////////////
    // Cameras
    ///////////////////////////
    ref<Camera> mp_editCamera;

    // TODO temporary, this should be part of scene only
    CameraCmp*  mp_sceneCamera2DCmp;
    ref<Camera> mp_sceneCamera2D;

    ///////////////////////////
    // Panels
    ///////////////////////////
    scope<ViewportPanel>       mp_viewportPanel;
    scope<SceneControlPanel>   mp_sceneControlPanel;
    scope<SceneHeirarchyPanel> mp_sceneHierarchyPanel;
    scope<RenderStatsPanel>    mp_renderStatsPanel;
    scope<EditCameraMenuPanel> mp_editCameraMenuPanel;


    FelixLayer() : Layer(Layer::Type::REGULAR, "Felix")
    {
    }

    virtual void onInsert() override
    {
        Log::info("Hello from Felix!");

        mp_appRef    = &Application::s_get();
        mp_appWinRef = &mp_appRef->getWindow();
        mp_appRef->setDrawPeriodLimit(0.00334f);

        GraphicsApi::setDepthTest(false);


        mp_scene = makeRef<Scene>();

        ///////////////////////////
        // Cameras
        ///////////////////////////
        // edit camera
        mp_editCamera       = makeRef<Camera>(Camera::Type::PERSPECTIVE);
        mp_editCamera->setAspectRatio(m_aspectRatio);
        mp_editCamera->setPosition({0.0f, 0.0f, 2.4125f});
        mp_editCamera->setYaw(-90.0f);
        mp_editCamera->setSpeed(5.0f);


        //scene camera
        auto sceneCameraEntity = mp_scene->addEntity("Scene Camera");
        mp_sceneCamera2D       = makeRef<Camera>(Camera::Type::ORTHOGRAPHIC);
        mp_sceneCamera2D->setAspectRatio(m_aspectRatio);
        mp_sceneCamera2D->setSpeed(5.0f);

        mp_sceneCamera2DCmp
            = &(sceneCameraEntity.addComponent<CameraCmp>(mp_sceneCamera2D));
        mp_sceneCamera2DCmp->primary = true;

        sceneCameraEntity.addComponent<WindowRefCmp>(mp_appWinRef);

        sceneCameraEntity.addComponent<nativeLogicCmp>()
            .bind<sceneCameraController>();

        ///////////////////////////
        // Panels
        ///////////////////////////
        mp_viewportPanel    = makeScope<ViewportPanel>();
        mp_sceneControlPanel  = makeScope<SceneControlPanel>();
        mp_sceneHierarchyPanel = makeScope<SceneHeirarchyPanel>(mp_scene);
        mp_renderStatsPanel = makeScope<RenderStatsPanel>();
        mp_editCameraMenuPanel
            = makeScope<EditCameraMenuPanel>(mp_editCamera.get());

        ///////////////////////////
        // Test Sprite
        ///////////////////////////
        auto spriteEntity1 = mp_scene->addEntity("Test Sprite 1");
        auto& transformCmp1 = spriteEntity1.addComponent<TransformCmp>();
        transformCmp1.setScale({0.5f, 0.5f, 1.0f});
        auto& spriteCmp1 = spriteEntity1.addComponent<SpriteCmp>();
        spriteCmp1.color = {0.0f, 1.0f, 0.0f, 1.0f};

        auto spriteEntity2 = mp_scene->addEntity("Test Sprite 2");
        auto& transformCmp2 = spriteEntity2.addComponent<TransformCmp>();
        transformCmp2.setScale({0.75f, 0.75f, 1.0f});
        auto& spriteCmp2 = spriteEntity2.addComponent<SpriteCmp>();
        spriteCmp2.color = {1.9f, 0.0f, 0.0f, 1.0f};

        ///////////////////////////
        // Text text
        ///////////////////////////
        mp_generalFont = makeRef<Font>(
            "../resources/fonts/Roboto/Roboto-Regular.ttf");

        Font::Format format;
        format.p_font  = mp_generalFont;
        format.fgColor = {0.0f, 0.5f, 0.7f, 1.0f};
        format.bgColor = {0.0f, 0.0f, 0.0f, 0.0f};
        format.kerning = 0.0f;

        auto  textEntity    = mp_scene->addEntity("Test Text");
        auto& transformCmp3 = textEntity.addComponent<TransformCmp>();
        transformCmp3.setScale({0.25f, 0.25f, 1.0f});
        transformCmp3.setTranslationX(-0.4f);
        transformCmp3.setTranslationY(0.30f);
        auto textCmp = textEntity.addComponent<TextCmp>("Bumbus", format);

        ///////////////////////////
        // Setup Framebuffers
        ///////////////////////////
        // multisampled buffer we render to
        FrameBuffer::Spec fbSpec;
        fbSpec.width   = m_viewportSize.x;
        fbSpec.height  = m_viewportSize.y;
        fbSpec.samples = 8;
        fbSpec.colorAttachments.push_back(
            Texture::Spec(fbSpec.width,
                          fbSpec.height,
                          8,
                          Texture::Format::RGBA,  // irrelevant
                          Texture::FormatInternal::RGBA8,
                          Texture::DataType::UNSIGNED_BYTE,  // irrelevant
                          Texture::FilterType::LINEAR,
                          Texture::FilterType::LINEAR,
                          Texture::WrapType::CLAMP_TO_EDGE,
                          Texture::WrapType::CLAMP_TO_EDGE,
                          Texture::WrapType::CLAMP_TO_EDGE));

        fbSpec.depthType = Texture::FormatInternal::DEPTH24_STENCIL8;

        mp_frameBuffer = FrameBuffer::s_create(fbSpec);

        // buffer we blit multisampled buffer to for output to screen
        FrameBuffer::Spec screenSpec;
        screenSpec.width   = m_viewportSize.x;
        screenSpec.height  = m_viewportSize.y;
        screenSpec.samples = 1;
        screenSpec.colorAttachments.push_back(
            Texture::Spec(screenSpec.width,
                          screenSpec.height,
                          1,
                          Texture::Format::RGBA,  // irrelevant
                          Texture::FormatInternal::RGBA8,
                          Texture::DataType::UNSIGNED_BYTE,  // irrelevant
                          Texture::FilterType::LINEAR,
                          Texture::FilterType::LINEAR,
                          Texture::WrapType::REPEAT,
                          Texture::WrapType::REPEAT,
                          Texture::WrapType::REPEAT));

        screenSpec.depthType = Texture::FormatInternal::NONE;

        mp_screenBuffer = FrameBuffer::s_create(screenSpec);

    }

    virtual void onRemove() override
    {
    }

    virtual void onUpdate(float deltaTime) override
    {
        if (mp_sceneControlPanel->getState() == SceneControlPanel::State::PLAY
            && m_sceneState != State::PLAY)
        {
            m_sceneState = State::PLAY;
            mp_scene->onStart();
        }
        else if (mp_sceneControlPanel->getState()
                     == SceneControlPanel::State::PAUSE
                 && m_sceneState != State::PAUSE)
        {
            m_sceneState = State::PAUSE;
            mp_scene->onStop();
        }

        if (m_sceneState == State::PLAY)
        {
            mp_scene->onUpdate(deltaTime);
        }
        else if (m_sceneState == State::PAUSE)
        {
            if (m_viewportHovered)
            {
                _handleKeyboardInput(deltaTime);
            }
        }
    }

    virtual void onDraw(float deltaTime) override
    {
        mp_frameBuffer->bind();
        GraphicsApi::clear();

        if (mp_renderStatsPanel->m_wireFrame != GraphicsApi::getWireframe())
        {
            GraphicsApi::setWireframe(mp_renderStatsPanel->m_wireFrame);
        }

        Renderer2D::s_resetStats();

        if (m_sceneState == State::PAUSE)
        {
            mp_scene->_onDrawEditor(mp_editCamera.get());
        }
        else
        {
            mp_scene->onDraw();
        }

        // turn it off for the blit
        if (mp_renderStatsPanel->m_wireFrame)
        {
            GraphicsApi::setWireframe(false);
        }

        mp_frameBuffer->blit(*mp_screenBuffer.get());

        NM_UNUSED(deltaTime);
 

    }
    virtual void onEvent(Event& event) override
    {
        Event::Type eventType = event.getEventType();

        // We only care about these events if we're paused
        if (m_sceneState == State::PAUSE)
        {
            if (eventType == Event::Type::MOUSEMOTION)
            {
                if (mp_appWinRef->mouseButtonPressed(MouseButton::MIDDLE))
                {
                    glm::vec2 delta = {(float)event.getDetails().motion.xrel,
                                       (float)event.getDetails().motion.yrel};

                    static const float orbitScale = 4.0f;
                    mp_editCamera->processViewUpdate(delta * orbitScale, true);
                }
            }
            else if (eventType == Event::Type::MOUSEWHEEL)
            {
                float zoomAmount = event.getDetails().wheel.preciseY;

                mp_editCamera->processZoom(zoomAmount);
            }
        }
    }

    void _handleKeyboardInput(float deltaTime)
    {
        ///////////////////////////
        // Camera Controls
        ///////////////////////////
        if (mp_appWinRef->keyPressed(ScanCode::W))
        {
            mp_editCamera->processPosiUpdate(Camera::Movement::FORWARD,
                                             deltaTime);
        }
        if (mp_appWinRef->keyPressed(ScanCode::S))
        {
            mp_editCamera->processPosiUpdate(Camera::Movement::BACKWARD,
                                             deltaTime);
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
            mp_editCamera->processPosiUpdate(Camera::Movement::RIGHT,
                                             deltaTime);
        }
    }

    void _makeDockspace()
    {
        static bool               dockspaceOpen           = true;
        static bool               optFullscreenPersistant = true;
        bool                      optFullscreen  = optFullscreenPersistant;
        static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent
        // window not dockable into, because it would be confusing to have two
        // docking targets within each others.
        ImGuiWindowFlags windowFlags
            = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (optFullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            windowFlags
                |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                   | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus
                           | ImGuiWindowFlags_NoNavFocus;
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
        float       minWinSizeX = style.WindowMinSize.x;
        style.WindowMinSize.x   = 250.0f;
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspaceId = ImGui::GetID("FelixDockSpace");
            ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);
        }
        style.WindowMinSize.x = minWinSizeX;
    }

    virtual void onGuiUpdate(float deltaTime) override
    {
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
                    // Handle when 'Open' is clicked
                }

                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {
                    // Handle when 'Save' is clicked
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
            // Viewport
            ///////////////////////////

            mp_viewportPanel->onDraw(
                mp_screenBuffer,
                mp_editCamera->getType() == Camera::Type::ORTHOGRAPHIC
                    ? &worldBounds
                    : nullptr);
            
            m_viewportFocused = mp_viewportPanel->m_viewportFocused;
            m_viewportHovered = mp_viewportPanel->m_viewportHovered;
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

            ///////////////////////////
            // Scene Control
            ///////////////////////////
            mp_sceneControlPanel->onDraw();

            ///////////////////////////
            // Scene Heirarchy
            ///////////////////////////
            mp_sceneHierarchyPanel->onDraw();

            ///////////////////////////
            // Camera Menu
            ///////////////////////////
            mp_editCameraMenuPanel->onDraw(worldBounds);

            ///////////////////////////
            // Render Stats
            ///////////////////////////
            mp_renderStatsPanel->onDraw(deltaTime);


            ImGui::End();  // dockspace
    }
};

////////////////////////////////////////////////////////////////////////////////
// Make the application and start it
////////////////////////////////////////////////////////////////////////////////
class Felix : public Application
{
   public:
    Felix() : Application("Felix", 1920, 1080)
    {
        Application::insertLayer(makeRef<FelixLayer>());
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