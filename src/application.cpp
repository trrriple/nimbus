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
    NM_LOG("------------------------------------------\n");
    NM_LOG("----- Nimbus Engine Application Init -----\n");
    NM_LOG("------------------------------------------\n");

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
    return m_quit;
}

void Application::execute()
{
    /* main game loop */
    while (!m_quit)
    {
        /* main event loop */
        _render();
        _processEvents();
        _onUpdate();
    }

    NM_LOG("Quitting\n");
}

void Application::addGuiCallback(nbCallback_t p_func)
{
    m_guiCallbacks.push_back(p_func);
}

void Application::addRenderCallback(nbCallback_t p_func)
{
    m_renderCallbacks.push_back(p_func);
}

void Application::addUpdateCallback(nbCallback_t p_func)
{
    m_updateCallbacks.push_back(p_func);
}

void Application::setEventCallback(nbWindowEvtCallback_t p_func)
{
    m_windowEvtCallback = p_func;
}

void Application::setEventCallback(nbMouseMotionEvtCallback_t p_func)
{
    m_mouseMotionEvtCallback = p_func;
}

void Application::setEventCallback(nbMouseWheelEvtCallback_t p_func)
{
    m_mouseWheelEvtCallback = p_func;
}

void Application::setEventCallback(nbMouseButtonEvtCallback_t p_func)
{
    m_mouseButtonEvtCallback = p_func;
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
        NM_ELOG(0, "SDL could not init. SDL_Error: %s\n", SDL_GetError());
        throw std::runtime_error("Failed initialize SDL\n");
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

    if (mp_window == nullptr)
    {
        NM_ELOG(
            0, "Window could not be created. sdl error %s\n", SDL_GetError());
        throw std::runtime_error("Window could not be created.\n");
    }
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

    // render callbacks
    for (auto& p_callback : m_renderCallbacks)
    {
        p_callback();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    mp_renderer->render();
}

void Application::_onUpdate()
{
    for (auto& p_callback : m_updateCallbacks)
    {
        p_callback();
    }
}

void Application::_processEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0)
    {
        ImGuiIO& io = ImGui::GetIO();

        switch (event.type)
        {
            case SDL_WINDOWEVENT:
            {
                if (m_windowEvtCallback != nullptr)
                {
                    m_windowEvtCallback(event.window);
                }

                // handle internal window stuff
                _processWindowEvents(event.window);

                break;
            }
            case SDL_MOUSEMOTION:
            {
                if (!io.WantCaptureMouse && m_mouseMotionEvtCallback != nullptr
                    && !m_menuMode)
                {
                    m_mouseMotionEvtCallback(event.motion);
                }

                break;
            }
            case SDL_MOUSEWHEEL:
            {
                if (m_mouseWheelEvtCallback != nullptr && !m_menuMode)
                {
                    m_mouseWheelEvtCallback(event.wheel);
                }

                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    if (!io.WantCaptureMouse)
                    {
                        // We must have clicked off a dearimgui window
                        setMenuMode(false);
                    }

                    if (m_mouseButtonEvtCallback != nullptr)
                    {
                        m_mouseButtonEvtCallback(event.button);
                    }
                }
                break;
            }
            case SDL_QUIT:
            {
                NM_LOG("Should quit\n");
                m_quit = true;
                break;
            }
            default:
            {
                break;
            }
        }

        // foward events to ingui
        ImGui_ImplSDL2_ProcessEvent(&event);
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

            NM_LOG("Window Resized %d x %d\n", evt.data1, evt.data2);
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
