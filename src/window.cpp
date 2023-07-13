#include "nmpch.hpp"
#include "core.hpp"

#include "window.hpp"
#include "platform/rendererApi.hpp"

namespace nimbus
{

Window::Window(const std::string& windowCaption,
               uint32_t           width,
               uint32_t           height)
    : m_width(width), m_height(height)
{
    NM_PROFILE_DETAIL();

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        NM_CORE_ASSERT(
            0, "SDL could not init. SDL_Error: %s\n", SDL_GetError());
    }

    // relative mouse
    SDL_SetRelativeMouseMode(SDL_TRUE);

    mp_window = SDL_CreateWindow(
        windowCaption.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        m_width,
        m_height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

    m_sdlWindowId = SDL_GetWindowID(mp_window);

    NM_CORE_ASSERT(mp_window,
                   "Window could not be created. sdl error %s\n",
                   SDL_GetError());
}

Window::~Window()
{
    NM_PROFILE_DETAIL();
    
    SDL_DestroyWindow(mp_window);
    mp_window = nullptr;

    NM_CORE_INFO("Window destroyed\n")

    // Quit SDL subsystems
    SDL_Quit();
}

void Window::graphicsContextInit()
{
    NM_PROFILE_DETAIL();

    m_context = SDL_GL_CreateContext(mp_window);
    NM_CORE_ASSERT(
        m_context, "Failed to created OpenGL Context %s\n", SDL_GetError());

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

    // Set V-sync
    SDL_GL_SetSwapInterval(m_VSyncOn);

    // setup GLAD
    if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
        NM_CORE_ASSERT_STATIC(0, "Failed to initialize Glad %s\n");
    }
}

void Window::setEventCallback(const nbWindowEvtCallback_t& callback)
{
    NM_PROFILE_TRACE();

    m_evtCallback = callback;
}

void Window::setExitCallback(const nbWindowEvtCallback_t& callback)
{
    NM_PROFILE_TRACE();

    m_exitCallback = callback;
}

void Window::onUpdate()
{
    NM_PROFILE();

    _pollEvents();
    SDL_GL_SwapWindow(mp_window);
    _calcFramerate();
}

bool Window::keyPressed(uint32_t keyCode)
{
    NM_PROFILE_DETAIL();

    const uint8_t* keyboardState = SDL_GetKeyboardState(nullptr);

    return keyboardState[keyCode];
}

void Window::setVSync(bool on)
{
    NM_PROFILE_TRACE();

    if (on != m_VSyncOn)
    {
        SDL_GL_SetSwapInterval(on);
        m_VSyncOn = on;
    }
}

bool Window::getVSync()
{
    return m_VSyncOn;
}

void Window::_handleWindowEvents()
{
    NM_PROFILE_DETAIL();

    switch (m_event.getEvent()->window.event)
    {
        // main window resized
        case (SDL_WINDOWEVENT_SIZE_CHANGED):
        {
            m_width  = m_event.getEvent()->window.data1;
            m_height = m_event.getEvent()->window.data2;

            RendererApi::setViewportSize(0, 0, m_width, m_height);

            NM_CORE_INFO("Window Resized %d x %d\n", m_width, m_height);
        }
        // main window closed
        case (SDL_WINDOWEVENT_CLOSE):
        {
            NM_CORE_INFO("This window closed\n");

            m_exitCallback(m_event);
        }
        default:
            break;
    }
}

void Window::_pollEvents()
{
    NM_PROFILE();

    m_event.clear();

    while (SDL_PollEvent(m_event.getEvent()))
    {
        // call the event callback for each event
        m_evtCallback(m_event);

        // handle internal window event stuff
        // SDL_QUIT comes if all windows are closed
        if (m_event.getEventType() == SDL_QUIT)
        {
            NM_CORE_INFO("SDL_QUIT\n");
            m_exitCallback(m_event);
        }
        else if (m_event.getEventType() == SDL_WINDOWEVENT)
        {
            // we only want to handle window events for this window
            if (m_event.getEvent()->window.windowID == m_sdlWindowId)
            {
                _handleWindowEvents();
            }
        }

        m_event.clear();
    }
}

void Window::_calcFramerate()
{
    NM_PROFILE_TRACE();

    float tNow_s = core::getTime_s();

    static uint32_t frameCount    = 0;
    static float    tLastFrame_s  = 0;
    static float    samplesPeriod = 0.0;

    m_tFrame_s   = tNow_s - tLastFrame_s;
    tLastFrame_s = tNow_s;

    frameCount++;

    samplesPeriod += m_tFrame_s;

    if (samplesPeriod >= 0.5)
    {
        m_fps         = (float)frameCount / (samplesPeriod);
        frameCount    = 0;
        samplesPeriod = 0;
    }
}

}  // namespace nimbus