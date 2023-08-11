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

    Window(const std::string& windowCaption,
           uint32_t           width,
           uint32_t           height) noexcept;
    
    ~Window() noexcept;

    void graphicsContextInit() noexcept;

    void setEventCallback(const WindowEventCallback_t& callback) noexcept;

    void setExitCallback(const WindowEventCallback_t& callback) noexcept;

    void swapBuffers() noexcept;

    void pumpEvents() noexcept;

    bool keyPressed(ScanCode scanCode) const noexcept;

    bool modKeyPressed(KeyMod keyMod) const noexcept;

    bool mouseButtonPressed(MouseButton button) const noexcept;

    glm::vec2 mousePos() const noexcept;

    float  mouseWheelPos() const noexcept;

    inline uint32_t getHeight() const noexcept
    {
        return m_height;
    }
    inline uint32_t getWidth() const noexcept
    {
        return m_width;
    }

    inline float getAspectRatio() const noexcept
    {
        return m_aspectRatio;
    }

    inline void* getOsWindow() const noexcept
    {
        return mp_window;
    }

    inline void* getContext() const noexcept
    {
        return mp_context;
    } 

    void setVSync(bool on) noexcept;

    inline bool getVSync() const noexcept
    {
        return m_VSyncOn;
    }

    inline bool isMinimized() const noexcept
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

    void _handleWindowEvents() noexcept;

    void _pollEvents() noexcept;

    void _calcFramerate() noexcept;
};

}  // namespace nimbus