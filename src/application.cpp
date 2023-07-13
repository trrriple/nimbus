#include "application.hpp"
#include "renderer/renderer.hpp"
#include "platform/rendererApi.hpp"

#include "nmpch.hpp"

namespace nimbus
{

Application::Application(const std::string& name,
                         int               windowWidth,
                         int               windowHeight,
                         bool              is3d)
    : m_name(name)
{
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

    NM_CORE_INFO("------------------------------------------\n");
    NM_CORE_INFO("----- Nimbus Engine Application Init -----\n");
    NM_CORE_INFO("------------------------------------------\n");

    NM_CORE_ASSERT(!sp_instance, "Application should only be created once!\n");

    sp_instance = this;


    mp_window = std::make_unique<Window>(m_name, windowWidth, windowHeight);
    mp_window->graphicsContextInit();
    
    Renderer::init();
    
    mp_window->setEventCallback(
        std::bind(&Application::onEvent, this, std::placeholders::_1));

    if (is3d)
    {
        RendererApi::setDepthTest(true);
        addGuiCallback(std::bind(&Application::_cameraMenu, this));
        mp_camera = std::make_unique<Camera>(glm::vec3(-10.0f, 0.0f, 0.0f));
    }
    else
    {
        RendererApi::setDepthTest(false);
    }

    _initGui();
    addGuiCallback(std::bind(&Application::_renderStatsDisplay, this));
}

Application::~Application()
{
}

void Application::onInit()
{
}

void Application::onExit()
{
    _killGui();
}

bool Application::shouldQuit() const
{
    return !m_Active;
}

void Application::execute()
{
    /* main game loop */
    while (m_Active)
    {
        /* main event loop */
        _render();
        mp_window->onUpdate();
    }

    NM_CORE_INFO("Quitting\n");
}

void Application::onEvent(Event& event)
{
    // interate over the layer deck backwards because we want layers at
    // the top of the deck to handle events first if they so choose
    // this is because when rendering layers, they are rendered bottom
    // to top so the last layer will be the layer "highest" in the scene
    for (auto it = m_layerDeck.rbegin(); it != m_layerDeck.rend(); ++it)
    {
        if (event.wasHandled())
        {
            break;
        }
        (*it)->onEvent(event);
    }

    ////////////////////////////////////////
    // Handle Application Centric Events
    ////////////////////////////////////////
    switch (event.getEventType())
    {
        case SDL_MOUSEBUTTONDOWN:
        {
            if (event.getEvent()->button.button == SDL_BUTTON_LEFT)
            {
                ImGuiIO& io = ImGui::GetIO();
                if (!io.WantCaptureMouse)
                {
                    // We must have clicked off a dearimgui window
                    setMenuMode(false);
                }
            }
            break;
        }
        case SDL_QUIT:
        {
            NM_CORE_INFO("Should quit\n");
            m_Active = false;
            break;
        }
        default:
        {
            break;
        }
    }

    // foward events to imgui
    ImGui_ImplSDL2_ProcessEvent(event.getEvent());
}

void Application::insertLayer(Layer* layer, int32_t location)
{
    m_layerDeck.insertLayer(layer, location);
}

void Application::removeLayer(Layer* layer)
{
    m_layerDeck.removeLayer(layer);
}

void Application::addGuiCallback(nbCallback_t p_func)
{
    m_guiCallbacks.push_back(p_func);
}


float Application::getFrametime() const
{
    return mp_window->m_tFrame_s;
}

const uint8_t* Application::getKeyboardState() const
{
    return SDL_GetKeyboardState(nullptr);
}

const glm::mat4 Application::getProjectionMatrix(bool perspective) const
{
    if (perspective)
    {
        return glm::perspective(
            glm::radians(mp_camera->m_fov),
            (float)mp_window->getWidth() / (float)mp_window->getHeight(),
            0.1f,
            300.0f);
    }
    else
    {
        return glm::ortho(0.0f,
                          (float)mp_window->getWidth(),
                          (float)mp_window->getHeight(),
                          0.0f,
                          -1.0f,
                          1.0f);
    }
}

const glm::mat4 Application::getViewMatrix() const
{
    return mp_camera->getViewMatrix();
}

void Application::cameraViewUpdate(float     xOffset,
                                   float     yOffset,
                                   GLboolean constrainPitch)
{
    mp_camera->processViewUpdate(xOffset, yOffset, constrainPitch);
}

void Application::cameraZoomUpdate(float yOffset)
{
    mp_camera->processZoom(yOffset);
}

void Application::cameraPosUpdate(Camera::Movement movement)
{
    mp_camera->processPosUpd(movement, getFrametime());
}

Window& Application::getWindow()
{
    return *mp_window;
}

void Application::setMenuMode(bool mode)
{
    // if mode doesn't change, don't do anything
    if (m_menuMode == mode)
    {
        return;
    }

    if (mode)
    {
        SDL_ShowCursor(SDL_ENABLE);
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }
    else
    {
        SDL_ShowCursor(SDL_DISABLE);
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    m_menuMode = mode;
}

bool Application::getMenuMode() const
{
    return m_menuMode;
}

void Application::_initGui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    UNUSED(io);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    // window is the SDL_Window*
    // context is the SDL_GLContext
    ImGui_ImplSDL2_InitForOpenGL(mp_window->getSDLWindow(),
                                 mp_window->getContext());
    ImGui_ImplOpenGL3_Init();
}

void Application::_killGui()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void Application::_render()
{
    RendererApi::clear();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(mp_window->getSDLWindow());
    ImGui::NewFrame();

    // gui callbacks
    for (auto& p_callback : m_guiCallbacks)
    {
        p_callback();
    }

    for (auto it = m_layerDeck.begin(); it != m_layerDeck.end(); ++it)
    {
        (*it)->onUpdate();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void Application::_renderStatsDisplay()
{
    ImGui::Begin("Render Status", 0, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Draw Parameters");

    bool newWireFrameMode = RendererApi::getWireframe();
    ImGui::Checkbox("Wireframe Mode", &newWireFrameMode);
    RendererApi::setWireframe(newWireFrameMode);

    ImGui::SameLine();

    bool newVsyncMode = mp_window->getVSync();
    ImGui::Checkbox("Vertical Sync", &newVsyncMode);
    mp_window->setVSync(newVsyncMode);

    ImGui::Text("Render Time: %.02f ms/frame (%.02f FPS)",
                getFrametime() * 1000.0f,
                mp_window->m_fps);

    ImGui::End();
}

void Application::_cameraMenu()
{
    ImGui::Begin("Camera Menu", 0, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Camera Pos (X: %.03f, Y: %.03f, Z: %.03f)",
                mp_camera->m_position[0],
                mp_camera->m_position[1],
                mp_camera->m_position[2]);

    ImGui::Text("Camera Attitude (Yaw: %.03f, Pitch: %.03f)",
                mp_camera->m_yaw,
                mp_camera->m_pitch);

    ImGui::SliderFloat("Speed", &mp_camera->m_speed, 1.0f, 100.0f);
    ImGui::SliderFloat("Sensitivity", &mp_camera->m_sensitivity, 0.01f, 0.5f);
    ImGui::End();
}

};  // namespace nimbus
