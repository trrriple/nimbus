#include "nimbus.hpp"
#include "nimbus/core/entry.hpp"

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
    inline static const uint32_t k_frameHistoryLength = 60 * 2 + 1;
   
   public:

    ///////////////////////////
    // References
    ///////////////////////////
    Application* mp_appRef;
    Window*      mp_appWinRef;

    ///////////////////////////
    // Scene
    ///////////////////////////
    ref<Scene>   m_scene;

    ///////////////////////////
    // Viewport Info
    ///////////////////////////
    glm::vec2 m_viewportSize    = {800, 600};
    float     m_aspectRatio     = 800 / 600;
    bool      m_wireFrame       = false;
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
    // Performance monitors
    ///////////////////////////
    std::vector<float> m_frameTimes_ms;


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

        m_frameTimes_ms.reserve(k_frameHistoryLength);


        ///////////////////////////
        // Cameras
        ///////////////////////////
        //scene camera
        auto sceneCameraEntity = m_scene->addEntity("Scene Camera");
        mp_sceneCamera2D       = makeRef<Camera>(false);
        mp_sceneCamera2D->setAspectRatio(m_aspectRatio);

        mp_sceneCamera2DCmp
            = &(sceneCameraEntity.addComponent<CameraCmp>(mp_sceneCamera2D));
        mp_sceneCamera2DCmp->renderWith = true;

        sceneCameraEntity.addComponent<WindowRefCmp>(mp_appWinRef);

        sceneCameraEntity.addComponent<nativeLogicCmp>()
            .bind<sceneCameraController>();

        // edit camera
        auto editCameraEntity = m_scene->addEntity("Edit Camera");
        mp_editCamera3D       = makeRef<Camera>(true);
        mp_editCamera3D->setAspectRatio(m_aspectRatio);
        mp_editCamera3D->setPosition({0.0f, 0.0f, 2.4125f});
        mp_editCamera3D->setYaw(-90.0f);
        mp_editCamera3DCmp
            = &(editCameraEntity.addComponent<CameraCmp>(mp_editCamera3D));
        mp_editCamera3DCmp->renderWith = false;

        editCameraEntity.addComponent<WindowRefCmp>(mp_appWinRef);

        editCameraEntity.addComponent<nativeLogicCmp>()
            .bind<editCameraController>();

        mp_currCamera = mp_sceneCamera2D;

        ///////////////////////////
        // Test Sprite
        ///////////////////////////
        auto quadEntity = m_scene->addEntity("Test Sprite");
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

        auto  textEntity   = m_scene->addEntity("Hello Text");
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
        m_scene->onStart();

    }

    virtual void onRemove() override
    {
    }

    virtual void onUpdate(float deltaTime) override
    {
        if (m_viewportHovered)
        {
            m_scene->onUpdate(deltaTime);
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

        Renderer2D::s_resetStats();
        m_scene->onDraw(deltaTime);

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
        NM_UNUSED(event);
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

        // Bounds used for some calculations
        Camera::Bounds worldBounds = mp_currCamera->getVisibleWorldBounds();

        ///////////////////////////
        // Render Stats
        ///////////////////////////
        char buf[128];
        snprintf(buf,
                 128,
                 "%.02f ms/frame (%.02f FPS)###RenderStatus",
                 deltaTime * 1000.0f,
                 mp_appWinRef->m_fps);

        ImGui::Begin(buf,
                     0,
                     ImGuiWindowFlags_AlwaysAutoResize
                         | ImGuiWindowFlags_NoFocusOnAppearing);

        bool newVsyncMode = mp_appWinRef->getVSync();
        if (ImGui::Checkbox("Vertical Sync", &newVsyncMode))
        {
            mp_appWinRef->setVSync(newVsyncMode);
        }

        ImGui::SameLine();

        ImGui::Checkbox("Wireframe Mode", &m_wireFrame);


        // this is kinda inefficient, could draw over instead of
        // scroll?
        if (m_frameTimes_ms.size() == m_frameTimes_ms.capacity())
        {
            m_frameTimes_ms.erase(m_frameTimes_ms.begin());
        }
        m_frameTimes_ms.push_back(deltaTime * 1000.0);

        ImGui::PlotLines("Frame Times",
                         m_frameTimes_ms.data(),
                         m_frameTimes_ms.size(),
                         0,
                         nullptr,
                         0.0f,
                         20.0f,
                         ImVec2(0, 0),
                         sizeof(float));

        if (ImGui::CollapsingHeader("Renderer2D Stats"))
        {
            Renderer2D::Stats stats = Renderer2D::s_getStats();

            ImGui::PushItemWidth(60.0f);
            ImGui::LabelText("Draw Calls", "%i", stats.drawCalls);
            ImGui::LabelText("Quads", "%i", stats.quads);
            ImGui::LabelText("Characters", "%i", stats.characters);

            ImGui::LabelText("Total Vertices", "%i", stats.totalVertices);
            ImGui::LabelText(
                "Quad Vertices Available", "%i", stats.quadVertsAvail);
            ImGui::LabelText(
                "Text Vertices Available", "%i", stats.textVertsAvail);

            ImGui::PopItemWidth();
        }

        if (ImGui::CollapsingHeader("Layers"))
        {
            if (ImGui::BeginTable("Layer Order", 2))
            {
                ImGui::TableSetupColumn("#");
                ImGui::TableSetupColumn("Name");
                ImGui::TableHeadersRow();

                auto layerNames = mp_appRef->getLayerDeck().getLayerNames();
                for (uint32_t i = 0; i < layerNames.size(); i++)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", i);
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", layerNames[i]->c_str());
                }

                ImGui::EndTable();
            }
        }

        ImGui::End();

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


        float titleBarHeight
            = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;

        ///////////////////////////
        // Draw the cursor pos
        ///////////////////////////
        if (m_viewportHovered)
        {
            ImVec2 mousePos = ImGui::GetMousePos();

            // Get the top-left corner of
            // the current ImGui window
            ImVec2 windowPos = ImGui::GetWindowPos();

            glm::vec2 mousePosInViewportPix
                = {mousePos.x - windowPos.x,
                   mousePos.y - windowPos.y - titleBarHeight};

            glm::vec2 mousePosInViewport = util::mapPixToScreen(
                {mousePosInViewportPix.x, mousePosInViewportPix.y},
                worldBounds.topLeft.x,
                worldBounds.topRight.x,
                worldBounds.topLeft.y,
                worldBounds.bottomLeft.y,
                m_viewportSize.x,
                m_viewportSize.y);


            // draw mouse position over image
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            char posString[32];

            snprintf(posString,
                     sizeof(posString),
                     "X:%.04f, Y:%.04f",
                     mousePosInViewport.x,
                     mousePosInViewport.y);

            ImVec2 texPos = {windowPos.x + m_viewportSize.x - 95.0f,
                             windowPos.y + m_viewportSize.y};

            // Draw the text on the draw list
            drawList->AddText(texPos, IM_COL32(255, 255, 255, 255), posString);
        }

        ImGui::End();  // viewport

        ///////////////////////////
        // Camera Menu
        ///////////////////////////
        ImGui::Begin("Camera Menu", 0, ImGuiWindowFlags_AlwaysAutoResize);


        if(ImGui::Button("Reset Camera"))
        {
            if(mp_currCamera == mp_editCamera3D)
            {
                mp_currCamera->setPosition({0.0f, 0.0f, 2.4125f});
                mp_currCamera->setYaw(-90.0f);
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