#include "application.hpp"

#include "nmpch.hpp"

namespace nimbus
{

Application::Application(const std::string name,
                         int               screenWidth,
                         int               screenHeight,
                         bool              is3d)
    : m_name(name), m_screenWidth(screenWidth), m_screenHeight(screenHeight)
{
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

    NM_CORE_INFO("------------------------------------------\n");
    NM_CORE_INFO("----- Nimbus Engine Application Init -----\n");
    NM_CORE_INFO("------------------------------------------\n");

    NM_CORE_ASSERT(!sp_instance, "Application should only be created once!\n");

    sp_instance = this;

    _initOsIntrf(name);

    mp_renderer = std::make_unique<Renderer>(mp_window);
    mp_renderer->init();

    if (is3d)
    {
        mp_renderer->setDepthTest(true);
        addGuiCallback(std::bind(&Application::_cameraMenu, this));
        mp_camera = std::make_unique<Camera>(glm::vec3(-10.0f, 0.0f, 0.0f));
    }
    else
    {
        mp_renderer->setDepthTest(false);
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
    _killOsIntrf();
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
        _processEvents();
    }

    NM_CORE_INFO("Quitting\n");
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


SDL_Window* Application::getWindow() const
{
    return mp_window;
}

float Application::getFrametime() const
{
    return mp_renderer->m_tFrame_s;
}

const uint8_t* Application::getKeyboardState() const
{
    return SDL_GetKeyboardState(nullptr);
}

const glm::mat4 Application::getProjectionMatrix(bool perspective) const
{
    if (perspective)
    {
        return glm::perspective(glm::radians(mp_camera->m_fov),
                                (float)m_screenWidth / (float)m_screenHeight,
                                0.1f,
                                300.0f);
    }
    else
    {
        return glm::ortho(0.0f,
                          (float)m_screenWidth,
                          (float)m_screenHeight,
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
    mp_camera->processPosUpd(movement, mp_renderer->m_tFrame_s);
}

int Application::getScreenHeight() const
{
    return m_screenHeight;
}

int Application::getScreenWidth() const
{
    return m_screenWidth;
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

void Application::_initOsIntrf(const std::string windowCaption)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        NM_CORE_ASSERT(
            0, "SDL could not init. SDL_Error: %s\n", SDL_GetError());
    }

    // relative mouse
    SDL_SetRelativeMouseMode(SDL_TRUE);

    // openGL context
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    int contextFlags = 0;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &contextFlags);
    contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);

    // depth buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    mp_window = SDL_CreateWindow(
        windowCaption.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        m_screenWidth,
        m_screenHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

    NM_CORE_ASSERT(mp_window,
                   "Window could not be created. sdl error %s\n",
                   SDL_GetError());
}

void Application::_killOsIntrf()
{
    SDL_DestroyWindow(mp_window);
    mp_window = nullptr;

    // Quit SDL subsystems
    SDL_Quit();
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
    ImGui_ImplSDL2_InitForOpenGL(mp_window, mp_renderer->m_context);
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
    mp_renderer->clear();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(mp_window);
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

    mp_renderer->render();
}

void Application::_processEvents()
{
    Event event;

    while (SDL_PollEvent(event.getEvent()) != 0)
    {
        ImGuiIO& io = ImGui::GetIO();

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
            case SDL_WINDOWEVENT:
            {
              
                // handle internal window stuff
                _processWindowEvents(event.getEvent()->window);

                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                if (event.getEvent()->button.button == SDL_BUTTON_LEFT)
                {
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

        // foward events to ingui
        ImGui_ImplSDL2_ProcessEvent(event.getEvent());
    }
}

void Application::_processWindowEvents(SDL_WindowEvent& evt)
{
    switch (evt.event)
    {
        case (SDL_WINDOWEVENT_SIZE_CHANGED):
        {
            glViewport(0, 0, evt.data1, evt.data2);

            m_screenWidth  = evt.data1;
            m_screenHeight = evt.data2;

            NM_CORE_INFO("Window Resized %d x %d\n", evt.data1, evt.data2);
        }
        default:
            break;
    }
}

void Application::_renderStatsDisplay()
{
    ImGui::Begin("Render Status", 0, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Draw Parameters");

    bool newWireFrameMode = mp_renderer->getWireframe();
    ImGui::Checkbox("Wireframe Mode", &newWireFrameMode);
    mp_renderer->setWireframe(newWireFrameMode);

    ImGui::SameLine();

    bool newVsyncMode = mp_renderer->getVSync();
    ImGui::Checkbox("Vertical Sync", &newVsyncMode);
    mp_renderer->setVSync(newVsyncMode);

    ImGui::Text("Render Time: %.02f ms/frame (%.02f FPS)",
                mp_renderer->m_tFrame_s * 1000.0f,
                mp_renderer->m_fps);

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
