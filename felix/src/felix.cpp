#include "nimbus.hpp"
#include "nimbus/core/entry.hpp"

namespace nimbus
{

////////////////////////////////////////////////////////////////////////////////
// Main application implementation is in this layer
////////////////////////////////////////////////////////////////////////////////
class FelixLayer : public Layer
{
   public:
    Application* mp_appRef;
    Window*      mp_appWinRef;

    ref<Scene> m_scene;

    glm::vec2 m_viewportSize    = {800, 600};
    float     m_aspectRatio     = 800 / 600;
    bool      m_wireFrame       = false;
    bool      m_viewportFocused = false;
    bool      m_viewportHovered = false;

    ///////////////////////////
    // Framebuffers
    ///////////////////////////
    ref<FrameBuffer> mp_frameBuffer;
    ref<FrameBuffer> mp_screenBuffer;

    CameraCmp* mp_sceneCamera2DCmp;
    Camera*    mp_sceneCamera2D;

    CameraCmp* mp_editCamera3DCmp;
    Camera*    mp_editCamera3D;

    Camera* mp_currCamera;

    FelixLayer() : Layer(Layer::Type::REGULAR, "Felix")
    {
    }

    virtual void onInsert() override
    {
        Log::info("Hello from Felix!");

        mp_appRef    = &Application::s_get();
        mp_appWinRef = &mp_appRef->getWindow();
        mp_appRef->setDrawPeriodLimit(0.0f);

        m_scene = makeRef<Scene>();

        // add our cameras
        {
            auto cameraEntity   = m_scene->addEntity();
            mp_sceneCamera2DCmp = &(cameraEntity.addComponent<CameraCmp>());
            mp_sceneCamera2DCmp->renderWith = true;
            mp_sceneCamera2D                = &mp_sceneCamera2DCmp->camera;
            mp_sceneCamera2D->setAspectRatio(m_aspectRatio);
        }

        {
            auto cameraEntity  = m_scene->addEntity();
            mp_editCamera3DCmp = &(cameraEntity.addComponent<CameraCmp>());
            mp_editCamera3DCmp->renderWith = false;
            mp_editCamera3D                = &mp_editCamera3DCmp->camera;
            mp_editCamera3D->setType(true);
            mp_editCamera3D->setAspectRatio(m_aspectRatio);
            mp_editCamera3D->setPosition({0.0f, 0.0f, -2.4125f});
            mp_editCamera3D->setYaw(90.0f);
        }

        mp_currCamera = mp_sceneCamera2D;

        auto quadEntity = m_scene->addEntity();

        auto transformCmp = quadEntity.addComponent<TransformCmp>();

        transformCmp.setScale({0.5f, 0.5f, 1.0f});

        auto& spriteCmp = quadEntity.addComponent<SpriteCmp>();

        spriteCmp.color = {0.0f, 1.0f, 0.0f, 1.0f};

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
        if (m_viewportFocused)
        {
            _processKeyboardState(deltaTime);
        }
    }

    virtual void onDraw(float deltaTime) override
    {
        mp_frameBuffer->bind();
        GraphicsApi::clear();

        if (m_wireFrame != GraphicsApi::getWireframe())
        {
            GraphicsApi::setWireframe(m_wireFrame);
        }

        m_scene->onUpdate(deltaTime);

        // turn it off for the blit
        if (m_wireFrame)
        {
            GraphicsApi::setWireframe(false);
        }

        mp_frameBuffer->blit(*mp_screenBuffer.get());

        NM_UNUSED(deltaTime);
    }
    virtual void onEvent(Event& event) override
    {
        Event::Type eventType = event.getEventType();

        switch (eventType)
        {
            case (Event::Type::MOUSEWHEEL):
            {
                if (m_viewportFocused)
                {
                    mp_currCamera->processZoom(
                        event.getDetails().wheel.preciseY);
                }
                break;
            }
            default:
                break;
        }

        if (mp_currCamera == mp_editCamera3D)
        {
            if (eventType == Event::Type::MOUSEMOTION)
            {
                if (m_viewportFocused
                    && mp_appWinRef->mouseButtonPressed(MouseButton::MIDDLE))
                {
                    const float orbitScale = 4.0f;

                    float xOffset
                        = (float)event.getDetails().motion.xrel * orbitScale;
                    float yOffset
                        = (float)event.getDetails().motion.yrel * orbitScale;

                    mp_currCamera->processViewUpdate(xOffset, yOffset, true);
                }
            }
        }
    }

    virtual void onGuiUpdate(float deltaTime) override
    {
        NM_UNUSED(deltaTime);

        // Note: Switch this to true to enable dockspace
        static bool               dockspaceOpen             = true;
        static bool               opt_fullscreen_persistant = true;
        bool                      opt_fullscreen  = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent
        // window not dockable into, because it would be confusing to have two
        // docking targets within each others.
        ImGuiWindowFlags window_flags
            = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags
                |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                   | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus
                            | ImGuiWindowFlags_NoNavFocus;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will
        // render our background and handle the pass-thru hole, so we ask
        // Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka
        // window is collapsed). This is because we want to keep our DockSpace()
        // active. If a DockSpace() is inactive, all active windows docked into
        // it will lose their parent and become undocked. We cannot preserve the
        // docking relationship between an active window and an inactive
        // docking, otherwise any change of dockspace/settings would lead to
        // windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiIO&    io          = ImGui::GetIO();
        ImGuiStyle& style       = ImGui::GetStyle();
        float       minWinSizeX = style.WindowMinSize.x;
        style.WindowMinSize.x   = 370.0f;
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("FelixDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        style.WindowMinSize.x = minWinSizeX;

        ///////////////////////////
        // Viewport
        ///////////////////////////
        ImGui::SetNextWindowSize(ImVec2(800.0, 600.0), ImGuiCond_FirstUseEver);
        ImGui::Begin("Viewport");

        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset    = ImGui::GetWindowPos();

        glm::vec2 m_viewPortRegion[2];

        m_viewPortRegion[0] = {viewportMinRegion.x + viewportOffset.x,
                               viewportMinRegion.y + viewportOffset.y};
        m_viewPortRegion[1] = {viewportMaxRegion.x + viewportOffset.x,
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
            m_aspectRatio  = m_viewportSize.x / m_viewportSize.y;

            mp_frameBuffer->resize(m_viewportSize.x, m_viewportSize.y);
            mp_screenBuffer->resize(m_viewportSize.x, m_viewportSize.y);

            m_scene->onResize(m_viewportSize.x, m_viewportSize.y);
        }

        uint64_t textureID = mp_screenBuffer->getTextureId();
        ImGui::Image(reinterpret_cast<void*>(textureID),
                     ImVec2{m_viewportSize.x, m_viewportSize.y},
                     ImVec2{0, 1},
                     ImVec2{1, 0});

        ImGui::End();  // viewport

        ///////////////////////////
        // Camera Menu
        ///////////////////////////
        ImGui::Begin("Camera Menu", 0, ImGuiWindowFlags_AlwaysAutoResize);

        if(ImGui::Button("Reset Camera"))
        {
            if(mp_currCamera == mp_editCamera3D)
            {
                mp_currCamera->setPosition({0.0f, 0.0f, -2.4125f});
                mp_currCamera->setYaw(90.0f);
                mp_currCamera->setPitch(0.0f);
                mp_currCamera->setFov(45.0f);
            }
            else
            {
                mp_currCamera->setPosition({0.0f, 0.0f, 0.0f});
                mp_currCamera->setYaw(0.0f);
                mp_currCamera->setPitch(0.0f);
                mp_currCamera->setZoom(1.0f);
            }
        }
        

        bool editorCamera = mp_currCamera == mp_editCamera3D;
        if (ImGui::Checkbox("Editor Camera", &editorCamera))
        {
            if (editorCamera)
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
        }

        ImGui::Text("Viewport dimensions %i, %i (%f)",
                    (int)m_viewportSize.x,
                    (int)m_viewportSize.y,
                    mp_currCamera->getAspectRatio());

        glm::vec3 cameraPos = mp_currCamera->getPosition();

        if (ImGui::DragFloat3("Camera Pos", glm::value_ptr(cameraPos), 0.1f))
            ;
        {
            mp_currCamera->setPosition(cameraPos);
        }

        ImGui::Spacing();

        ImGui::Text("Pitch %.02f, Yaw %.02f, Zoom %.02f, FOV %.02f",
                    mp_currCamera->getPitch(),
                    mp_currCamera->getYaw(),
                    mp_currCamera->getZoom(),
                    mp_currCamera->getFov());

        if (ImGui::CollapsingHeader("Visible World Bounds"))
        {
            Camera::Bounds worldBounds = mp_currCamera->getVisibleWorldBounds();

            ImGui::BeginTable(
                "Visible World Bounds", 2, ImGuiTableFlags_Borders);

            // Row 1
            ImGui::TableNextRow();

            // Column 1, Row 1
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("(%.02f, %.02f %.02f)",
                        worldBounds.topLeft.x,
                        worldBounds.topLeft.y,
                        worldBounds.topLeft.z);

            // Column 2, Row 1
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("(%.02f, %.02f %.02f)",
                        worldBounds.topRight.x,
                        worldBounds.topRight.y,
                        worldBounds.topRight.z);

            // Row 2
            ImGui::TableNextRow();

            // Column 1, Row 2
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("(%.02f, %.02f %.02f)",
                        worldBounds.bottomLeft.x,
                        worldBounds.bottomLeft.y,
                        worldBounds.bottomLeft.z);

            // Column 2, Row 2
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("(%.02f, %.02f %.02f)",
                        worldBounds.bottomRight.x,
                        worldBounds.bottomRight.y,
                        worldBounds.bottomLeft.z);

            // End the table
            ImGui::EndTable();
        }

        ImGui::End();  // camera menu

        ///////////////////////////
        // General Options
        ///////////////////////////
        ImGui::Begin("Options", 0, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Checkbox("Wireframe Mode", &m_wireFrame);

        ImGui::End();  // options

        ImGui::End();  // dockspace
    }

    void _processKeyboardState(float deltaTime)
    {
        if (mp_currCamera == mp_sceneCamera2D)
        {
            if (mp_appWinRef->keyPressed(ScanCode::W))
            {
                mp_currCamera->processPosiUpdate(Camera::Movement::UP,
                                                 deltaTime);
            }
            if (mp_appWinRef->keyPressed(ScanCode::A))
            {
                mp_currCamera->processPosiUpdate(Camera::Movement::BACKWARD,
                                                 deltaTime);
            }
            if (mp_appWinRef->keyPressed(ScanCode::S))
            {
                mp_currCamera->processPosiUpdate(Camera::Movement::DOWN,
                                                 deltaTime);
            }
            if (mp_appWinRef->keyPressed(ScanCode::D))
            {
                mp_currCamera->processPosiUpdate(Camera::Movement::FORWARD,
                                                 deltaTime);
            }
        }
        else if (mp_currCamera == mp_editCamera3D)
        {
            if (mp_appWinRef->keyPressed(ScanCode::W))
            {
                mp_currCamera->processPosiUpdate(Camera::Movement::FORWARD,
                                                 deltaTime);
            }
            if (mp_appWinRef->keyPressed(ScanCode::S))
            {
                mp_currCamera->processPosiUpdate(Camera::Movement::BACKWARD,
                                                 deltaTime);
            }
            if (mp_appWinRef->keyPressed(ScanCode::SPACE))
            {
                mp_currCamera->processPosiUpdate(Camera::Movement::UP,
                                                 deltaTime);
            }
            if (mp_appWinRef->keyPressed(ScanCode::C))
            {
                mp_currCamera->processPosiUpdate(Camera::Movement::DOWN,
                                                 deltaTime);
            }
            if (mp_appWinRef->keyPressed(ScanCode::A))
            {
                mp_currCamera->processPosiUpdate(Camera::Movement::LEFT,
                                                 deltaTime);
            }
            if (mp_appWinRef->keyPressed(ScanCode::D))
            {
                mp_currCamera->processPosiUpdate(Camera::Movement::RIGHT,
                                                 deltaTime);
            }
        }
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