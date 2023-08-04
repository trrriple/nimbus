#include "nimbus.hpp"
#include "nimbus/core/entry.hpp"
#include "panels/viewportPanel.hpp"
#include "panels/renderStatsPanel.hpp"
#include "panels/cameraMenuPanel.hpp"
#include "panels/sceneHierarchyPanel.hpp"


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
    }

    virtual void onDestroy()
    {
    }

    virtual void onUpdate(float deltaTime)
    {
        // don't update this camera if we aren't using it
        if (!mp_cameraCmp->renderWith)
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

class editCameraController : public EntityLogic
{
   public:
    virtual void onCreate()
    {
        mp_cameraCmp = &getComponent<CameraCmp>();
        mp_window    = getComponent<WindowRefCmp>().p_window;

        m_lastMousePos = mp_window->mousePos();
    }

    virtual void onDestroy()
    {
    }

    virtual void onUpdate(float deltaTime)
    {
        // don't update this camera if we aren't using it
        if (!mp_cameraCmp->renderWith)
        {
            return;
        }

        if (mp_window->keyPressed(ScanCode::W))
        {
            mp_cameraCmp->p_camera->processPosiUpdate(Camera::Movement::FORWARD,
                                                      deltaTime);
        }
        if (mp_window->keyPressed(ScanCode::S))
        {
            mp_cameraCmp->p_camera->processPosiUpdate(
                Camera::Movement::BACKWARD, deltaTime);
        }
        if (mp_window->keyPressed(ScanCode::SPACE))
        {
            mp_cameraCmp->p_camera->processPosiUpdate(Camera::Movement::UP,
                                                      deltaTime);
        }
        if (mp_window->keyPressed(ScanCode::C))
        {
            mp_cameraCmp->p_camera->processPosiUpdate(Camera::Movement::DOWN,
                                                      deltaTime);
        }
        if (mp_window->keyPressed(ScanCode::A))
        {
            mp_cameraCmp->p_camera->processPosiUpdate(Camera::Movement::LEFT,
                                                      deltaTime);
        }
        if (mp_window->keyPressed(ScanCode::D))
        {
            mp_cameraCmp->p_camera->processPosiUpdate(Camera::Movement::RIGHT,
                                                      deltaTime);
        }

        glm::vec2 curMousePos = mp_window->mousePos();

        if (curMousePos != m_lastMousePos)
        {
            glm::vec2 delta = curMousePos - m_lastMousePos;
            m_lastMousePos  = curMousePos;

            if (mp_window->mouseButtonPressed(MouseButton::MIDDLE))
            {
                static const float orbitScale = 4.0f;
                mp_cameraCmp->p_camera->processViewUpdate(delta * orbitScale,
                                                          true);
            }
        }
    }

   private:
    CameraCmp* mp_cameraCmp;
    Window*    mp_window;
    glm::vec2  m_lastMousePos;
};

////////////////////////////////////////////////////////////////////////////////
// Main application implementation is in this layer
////////////////////////////////////////////////////////////////////////////////
class FelixLayer : public Layer
{   
   public:

    ///////////////////////////
    // References
    ///////////////////////////
    Application* mp_appRef;
    Window*      mp_appWinRef;

    ///////////////////////////
    // Scene
    ///////////////////////////
    ref<Scene>   mp_scene;

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
    CameraCmp*  mp_sceneCamera2DCmp;
    ref<Camera> mp_sceneCamera2D;
    CameraCmp*  mp_editCamera3DCmp;
    ref<Camera> mp_editCamera3D;
    ref<Camera> mp_currCamera;

    ///////////////////////////
    // Panels
    ///////////////////////////
    scope<ViewportPanel>       mp_viewportPanel;
    scope<RenderStatsPanel>    mp_renderStatsPanel;
    scope<SceneHeirarchyPanel> mp_sceneHierarchyPanel;
    scope<CameraMenuPanel>     mp_cameraMenuPanel;

    FelixLayer() : Layer(Layer::Type::REGULAR, "Felix")
    {
    }

    virtual void onInsert() override
    {
        Log::info("Hello from Felix!");

        mp_appRef    = &Application::s_get();
        mp_appWinRef = &mp_appRef->getWindow();
        mp_appRef->setDrawPeriodLimit(0.0f);

        mp_scene = makeRef<Scene>();

        ///////////////////////////
        // Cameras
        ///////////////////////////
        //scene camera
        auto sceneCameraEntity = mp_scene->addEntity("Scene Camera");
        mp_sceneCamera2D       = makeRef<Camera>(false);
        mp_sceneCamera2D->setAspectRatio(m_aspectRatio);
        mp_sceneCamera2D->setSpeed(5.0f);

        mp_sceneCamera2DCmp
            = &(sceneCameraEntity.addComponent<CameraCmp>(mp_sceneCamera2D));
        mp_sceneCamera2DCmp->renderWith = true;

        sceneCameraEntity.addComponent<WindowRefCmp>(mp_appWinRef);

        sceneCameraEntity.addComponent<nativeLogicCmp>()
            .bind<sceneCameraController>();

        // edit camera
        auto editCameraEntity = mp_scene->addEntity("Edit Camera");
        mp_editCamera3D       = makeRef<Camera>(true);
        mp_editCamera3D->setAspectRatio(m_aspectRatio);
        mp_editCamera3D->setPosition({0.0f, 0.0f, 2.4125f});
        mp_editCamera3D->setYaw(-90.0f);
        mp_editCamera3D->setSpeed(5.0f);

        mp_editCamera3DCmp
            = &(editCameraEntity.addComponent<CameraCmp>(mp_editCamera3D));
        mp_editCamera3DCmp->renderWith = false;

        editCameraEntity.addComponent<WindowRefCmp>(mp_appWinRef);

        editCameraEntity.addComponent<nativeLogicCmp>()
            .bind<editCameraController>();

        mp_currCamera = mp_sceneCamera2D;

        ///////////////////////////
        // Panels
        ///////////////////////////
        mp_viewportPanel    = makeScope<ViewportPanel>();
        mp_renderStatsPanel = makeScope<RenderStatsPanel>();
        mp_sceneHierarchyPanel = makeScope<SceneHeirarchyPanel>(mp_scene);
        mp_cameraMenuPanel  = makeScope<CameraMenuPanel>(mp_sceneCamera2D.get(),
                                                        mp_editCamera3D.get());

        

        ///////////////////////////
        // Test Sprite
        ///////////////////////////
        auto quadEntity = mp_scene->addEntity("Test Sprite");
        auto& transformCmp = quadEntity.addComponent<TransformCmp>();
        transformCmp.setScale({0.5f, 0.5f, 1.0f});
        auto& spriteCmp = quadEntity.addComponent<SpriteCmp>();
        spriteCmp.color = {0.0f, 1.0f, 0.0f, 1.0f};

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

        auto  textEntity   = mp_scene->addEntity("Hello Text");
        auto& transformCmp2 = textEntity.addComponent<TransformCmp>();
        transformCmp2.setScale({0.25f, 0.25f, 1.0f});
        transformCmp2.setTranslationX(-0.25f);
        transformCmp2.setTranslationY(0.30f);
        auto textCmp = textEntity.addComponent<TextCmp>("Hello", format);

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


        // TODO: this is part of stop/start
        mp_scene->onStart();

    }

    virtual void onRemove() override
    {
    }

    virtual void onUpdate(float deltaTime) override
    {
        if (m_viewportHovered)
        {
            mp_scene->onUpdate(deltaTime);
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
        mp_scene->onDraw(deltaTime);

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
        NM_UNUSED(event);
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
        // Bounds used for some calculations
        Camera::Bounds worldBounds = mp_currCamera->getVisibleWorldBounds();

        ///////////////////////////
        // Dockspace
        ///////////////////////////
        _makeDockspace();

        ///////////////////////////
        // Viewport
        ///////////////////////////
        mp_viewportPanel->onDraw(mp_screenBuffer, worldBounds);
        m_viewportFocused = mp_viewportPanel->m_viewportFocused;
        m_viewportHovered = mp_viewportPanel->m_viewportHovered;
        if (mp_viewportPanel->wasResized())
        {
            // we need to resize some stuff
            m_viewportSize = mp_viewportPanel->m_viewportSize;
            m_aspectRatio  = m_viewportSize.x / m_viewportSize.y;

            mp_frameBuffer->resize(m_viewportSize.x, m_viewportSize.y);
            mp_screenBuffer->resize(m_viewportSize.x, m_viewportSize.y);

            mp_scene->onResize(m_viewportSize.x, m_viewportSize.y);
        }

        ///////////////////////////
        // Camera Menu
        ///////////////////////////
        mp_cameraMenuPanel->onDraw(
            mp_currCamera.get(), worldBounds, m_viewportSize);

        // TODO: remove
        if (mp_cameraMenuPanel->m_useEditorCamera)
        {
            mp_currCamera                   = mp_editCamera3D;
            mp_editCamera3DCmp->renderWith  = true;
            mp_sceneCamera2DCmp->renderWith = false;
        }
        else
        {
            mp_currCamera                   = mp_sceneCamera2D;
            mp_editCamera3DCmp->renderWith  = false;
            mp_sceneCamera2DCmp->renderWith = true;
        }

        ///////////////////////////
        // Render Stats
        ///////////////////////////
        mp_renderStatsPanel->onDraw(deltaTime);


        ///////////////////////////
        // Scene Heirarchy
        ///////////////////////////
        mp_sceneHierarchyPanel->onDraw();

        ImGui::End();  // dockspace
    }
};

////////////////////////////////////////////////////////////////////////////////
// Make the application and start it
////////////////////////////////////////////////////////////////////////////////
class Felix : public Application
{
   public:
    Felix() : Application("Felix", 1280, 720)
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