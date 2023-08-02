#include "nimbus.hpp"
#include "nimbus/entry.hpp"

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
    glm::vec2    m_viewportSize = {800, 600};
    float        m_aspectRatio  = 800 / 600;
    bool         m_wireFrame    = false;

    ///////////////////////////
    // Framebuffers
    ///////////////////////////
    ref<FrameBuffer> mp_frameBuffer;
    ref<FrameBuffer> mp_screenBuffer;

    scope<Camera> mp_camera2D;

    FelixLayer() : Layer(Layer::Type::REGULAR, "Felix")
    {
    }

    virtual void onInsert() override
    {
        Log::info("Hello from Felix!");

        mp_appRef    = &Application::s_get();
        mp_appWinRef = &mp_appRef->getWindow();

        mp_appRef->setDrawPeriodLimit(0.0f);

        mp_camera2D = makeScope<Camera>(m_aspectRatio);

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
        _processKeyboardState(deltaTime);
    }

    virtual void onDraw(float deltaTime) override
    {
        mp_frameBuffer->bind();
        GraphicsApi::clear();

        if(m_wireFrame != GraphicsApi::getWireframe())
        {
            GraphicsApi::setWireframe(m_wireFrame);
        }

        Renderer2D::s_begin(*mp_camera2D);

        util::Transform transform;

        transform.translation = {-1.0, -1.0, 0.0f};
        transform.scale = {0.1f, 0.1f, 1.0f};
        transform.rotation = {0.0f, 0.0f, glm::radians(45.0f)};


        glm::vec4 color     = glm::vec4(0.7f, 0.0f, 0.5f, 1.0f);

        const int quadsPerSide = 50;
        const float padding = 0.17;
        for(int x = 0; x < quadsPerSide; x++)
        {
            transform.translation.y = -1.0;

            for(int y = 0; y < quadsPerSide; y++)
            {
                transform.translation.y += (-1.0 / 50) + padding;
                Renderer2D::s_drawQuad(transform.getModel(), color);

            }

            transform.translation.x += (-1.0 / 50) + padding;
        }

        Renderer2D::s_end();

        // turn it off for the blit
        if(m_wireFrame)
        {
            GraphicsApi::setWireframe(false);
        }

        mp_frameBuffer->blit(*mp_screenBuffer.get());

        UNUSED(deltaTime);
    }
    virtual void onEvent(Event& event) override
    {
        Event::Type eventType = event.getEventType();

        switch(eventType)
        {
            case(Event::Type::MOUSEWHEEL):
            {
                
                const float zoomScale = 0.25;
                float       zoomAmount
                    = event.getDetails().wheel.preciseY * zoomScale;

                float zoom = mp_camera2D->getZoom() - zoomAmount ;

                if(zoom <= 0.1)
                {
                    zoom = 0.1;
                }

                mp_camera2D->setZoom(zoom);
                break;
            }
            default:
                break;
        }
    }

    virtual void onGuiUpdate(float deltaTime) override
    {
        UNUSED(deltaTime);

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

        bool m_viewportFocused = ImGui::IsWindowFocused();
        bool m_viewportHovered = ImGui::IsWindowHovered();

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
            mp_camera2D->setAspectRatio(m_aspectRatio);
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

        glm::vec3 cameraPos = mp_camera2D->getPosition();

        float xPos  = cameraPos.x;
        float yPos  = cameraPos.y;
        float zPos  = cameraPos.z;

        ImGui::InputFloat("X", &xPos);
        ImGui::InputFloat("Y", &yPos);
        ImGui::InputFloat("Z", &zPos);

        ImGui::Spacing();


        float pitch = mp_camera2D->getPitch();
        float yaw   = mp_camera2D->getYaw();
        ImGui::Text("Pitch %.02f, Yaw %.02f, Zoom %.02f",
                    pitch,
                    yaw,
                    mp_camera2D->getZoom());

        if (xPos != cameraPos.x || yPos != cameraPos.y || zPos != cameraPos.z)
        {
            mp_camera2D->setPosition({xPos, yPos, zPos});
        }

        if (ImGui::CollapsingHeader("Visible World Bounds"))
        {
            Camera::Bounds worldBounds = mp_camera2D->getVisibleWorldBounds();

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
        if (mp_appWinRef->keyPressed(ScanCode::W))
        {
            mp_camera2D->processPosiUpdate(Camera::Movement::UP, deltaTime);
        }
        if (mp_appWinRef->keyPressed(ScanCode::A))
        {
            mp_camera2D->processPosiUpdate(Camera::Movement::BACKWARD, deltaTime);
        }
        if (mp_appWinRef->keyPressed(ScanCode::S))
        {
            mp_camera2D->processPosiUpdate(Camera::Movement::DOWN, deltaTime);
        }
        if (mp_appWinRef->keyPressed(ScanCode::D))
        {
            mp_camera2D->processPosiUpdate(Camera::Movement::FORWARD, deltaTime);
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