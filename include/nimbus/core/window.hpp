#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include "nimbus/core/event.hpp"
#include "nimbus/core/keyCode.hpp"
#include "nimbus/core/mouseButton.hpp"

#include "glm.hpp"

namespace nimbus
{

class Window : public refCounted
{
    typedef std::function<void(Event&)> WindowEventCallback_t;

   public:
    float m_tFrame_s = 0.0;
    float m_fps      = 0.0;

    Window(const std::string& windowCaption, uint32_t width, uint32_t height);

    ~Window();

    void graphicsContextInit();

    void setEventCallback(const WindowEventCallback_t& callback);

    void setExitCallback(const WindowEventCallback_t& callback);

    void swapBuffers();

    void pumpEvents();

    bool keyPressed(ScanCode scanCode) const;

    bool modKeyPressed(KeyMod keyMod) const;

    bool mouseButtonPressed(MouseButton button) const;

    glm::vec2 mousePos() const;

    float mouseWheelPos() const;

    inline uint32_t getHeight() const
    {
        return m_height;
    }
    inline uint32_t getWidth() const
    {
        return m_width;
    }

    inline float getAspectRatio() const
    {
        return m_aspectRatio;
    }

    inline void* getOsWindow() const
    {
        return mp_window;
    }

    inline void* getContext() const
    {
        return mp_context;
    }

    void setVSync(bool on);

    inline bool getVSync() const
    {
        return m_VSyncOn;
    }

    inline bool isMinimized() const
    {
        return m_minimized;
    }

   private:
    void* mp_window  = nullptr;
    void* mp_context = nullptr;

    WindowEventCallback_t m_evtCallback;
    WindowEventCallback_t m_exitCallback;

    Event m_event;

    uint32_t m_windowId;
    uint32_t m_width;
    uint32_t m_height;
    float    m_aspectRatio;
    bool     m_VSyncOn       = true;
    float    m_mouseWheelPos = 0.0f;
    bool     m_minimized     = false;

    void _handleWindowEvents();

    void _pollEvents();

    void _calcFramerate();
};

}  // namespace nimbus