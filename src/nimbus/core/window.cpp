#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/core/window.hpp"

#include "nimbus/core/keyCode.hpp"
#include "nimbus/core/mouseButton.hpp"
#include "nimbus/renderer/graphicsApi.hpp"
#include "nimbus/renderer/renderer.hpp"

namespace nimbus
{

Window::Window(const std::string& windowCaption, u32_t width, u32_t height)
    : m_width(width), m_height(height), m_aspectRatio(static_cast<f32_t>(width) / static_cast<f32_t>(height))
{
    NB_PROFILE_DETAIL();

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        NB_CORE_ASSERT(0, "SDL could not init. SDL_Error: %s", SDL_GetError());
    }

    // relative mouse
    SDL_SetRelativeMouseMode(SDL_FALSE);

    mp_window = static_cast<void*>(SDL_CreateWindow(windowCaption.c_str(),
                                                    SDL_WINDOWPOS_UNDEFINED,
                                                    SDL_WINDOWPOS_UNDEFINED,
                                                    m_width,
                                                    m_height,
                                                    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL));

    m_windowId = SDL_GetWindowID(static_cast<SDL_Window*>(mp_window));

    NB_CORE_ASSERT(mp_window, "Window could not be created. sdl error %s", SDL_GetError());
}

Window::~Window()
{
    NB_PROFILE_DETAIL();

    SDL_DestroyWindow(static_cast<SDL_Window*>(mp_window));
    mp_window = nullptr;

    Log::coreInfo("Window destroyed");

    // Quit SDL subsystems
    SDL_Quit();
}

void Window::graphicsContextInit()
{
    NB_PROFILE_DETAIL();

#ifdef NIMBUS_GL_DEBUG
    int contextFlags = 0;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &contextFlags);
    contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);
    // openGL context
#endif

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    // depth buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    mp_context = static_cast<void*>(SDL_GL_CreateContext(static_cast<SDL_Window*>(mp_window)));
    NB_CORE_ASSERT(mp_context, "Failed to created OpenGL Context %s", SDL_GetError());

    // Set V-sync
    SDL_GL_SetSwapInterval(m_VSyncOn);
}

void Window::setEventCallback(const WindowEventCallback_t& callback)
{
    NB_PROFILE_TRACE();

    m_evtCallback = callback;
}

void Window::setExitCallback(const WindowEventCallback_t& callback)
{
    NB_PROFILE_TRACE();

    m_exitCallback = callback;
}

void Window::swapBuffers()
{
    static SDL_Window* p_window = static_cast<SDL_Window*>(mp_window);

    Renderer::s_submit(+[]() { SDL_GL_SwapWindow(p_window); });

    _calcFramerate();
}

void Window::pumpEvents()
{
    NB_PROFILE();

    m_event.clear();

    while (SDL_PollEvent(reinterpret_cast<SDL_Event*>(&m_event.getDetails())))
    {
        // handle internal window event stuff
        // SDL_QUIT comes if all windows are closed
        switch (m_event.getEventType())
        {
            case (Event::Type::QUIT):
            {
                m_exitCallback(m_event);
                break;
            }
            case (Event::Type::WINDOW):
            {
                // we only want to handle window events for this window
                if (m_event.getDetails().window.windowID == m_windowId)
                {
                    _handleWindowEvents();
                }
                break;
            }
            case (Event::Type::MOUSEWHEEL):
            {
                // track the mouse wheel because SDL doesn't g78ive us a way
                // to get current position, just event based relative
                // position
                m_mouseWheelPos += m_event.getDetails().wheel.preciseY;
            }
            default:
                break;
        }

        // call the event callback for each event
        m_evtCallback(m_event);

        m_event.clear();
    }
}

bool Window::keyPressed(ScanCode scanCode) const
{
    NB_PROFILE_TRACE();

    const u8_t* keyboardState = SDL_GetKeyboardState(nullptr);

    // Will break if SDL changes their keymap
    return keyboardState[static_cast<u32_t>(scanCode)];
}

bool Window::modKeyPressed(KeyMod keyMod) const
{
    NB_PROFILE_TRACE();

    // Will break if SDL changes their keymap
    return SDL_GetModState() & static_cast<u32_t>(keyMod);
}

bool Window::mouseButtonPressed(MouseButton button) const
{
    NB_PROFILE_TRACE();

    const u32_t mouseState = SDL_GetMouseState(nullptr, nullptr);

    // Will break if SDL changes their button map
    return mouseState & SDL_BUTTON(static_cast<u32_t>(button));
}

glm::vec2 Window::mousePos() const
{
    NB_PROFILE_TRACE();

    int xPos;
    int yPos;

    SDL_GetMouseState(&xPos, &yPos);

    return {xPos, yPos};
}

f32_t Window::mouseWheelPos() const
{
    return m_mouseWheelPos;
}

void Window::setVSync(bool on)
{
    NB_PROFILE_TRACE();

    if (on != m_VSyncOn)
    {
        SDL_GL_SetSwapInterval(on);
        m_VSyncOn = on;
    }
}

void Window::_handleWindowEvents()
{
    NB_PROFILE_DETAIL();

    switch (m_event.getDetails().window.event)
    {
        // main window resized
        case (SDL_WINDOWEVENT_SIZE_CHANGED):
        {
            m_width  = m_event.getDetails().window.data1;
            m_height = m_event.getDetails().window.data2;

            m_aspectRatio = static_cast<f32_t>(m_width) / static_cast<f32_t>(m_height);

            GraphicsApi::setViewportSize(0, 0, m_width, m_height);

            Log::coreInfo("Window Resized %d x %d", m_width, m_height);
            break;
        }
        case (SDL_WINDOWEVENT_MINIMIZED):
        {
            m_minimized = true;
            break;
        }
        case (SDL_WINDOWEVENT_MAXIMIZED):
        case (SDL_WINDOWEVENT_RESTORED):
        {
            m_minimized = false;
            break;
        }  // main window closed
        case (SDL_WINDOWEVENT_CLOSE):
        {
            Log::coreInfo("This window closed");

            m_exitCallback(m_event);
        }
        default:
            break;
    }
}

void Window::_calcFramerate()
{
    NB_PROFILE_TRACE();

    f32_t tNow_s = core::getTime_s();

    static u32_t frameCount    = 0;
    static f32_t tLastFrame_s  = 0;
    static f32_t samplesPeriod = 0.0;

    m_tFrame_s   = tNow_s - tLastFrame_s;
    tLastFrame_s = tNow_s;

    frameCount++;

    samplesPeriod += m_tFrame_s;

    if (samplesPeriod >= 0.5)
    {
        m_fps         = (f32_t)frameCount / (samplesPeriod);
        frameCount    = 0;
        samplesPeriod = 0;
    }
}

}  // namespace nimbus