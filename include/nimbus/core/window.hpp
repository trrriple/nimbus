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

class Window
{
    typedef std::function<void(Event&)> nbWindowEvtCallback_t;

   public:
    float m_tFrame_s = 0.0;
    float m_fps      = 0.0;

    Window(const std::string& windowCaption, uint32_t width, uint32_t height);
    ~Window();

    void graphicsContextInit();

    void setEventCallback(const nbWindowEvtCallback_t& callback);

    void setExitCallback(const nbWindowEvtCallback_t& callback);

    void onUpdate();

    bool keyPressed(ScanCode scanCode) const;

    bool modKeyPressed(KeyMod keyMod) const;

    bool mouseButtonPressed(MouseButton button) const;

    glm::vec2 mousePos() const;

    float  mouseWheelPos() const;

    uint32_t getHeight() const
    {
        return m_height;
    }
    uint32_t getWidth() const
    {
        return m_width;
    }

    float getAspectRatio() const
    {
        return m_aspectRatio;
    }

    void* getOsWindow() const
    {
        return mp_window;
    }

    void* getContext() const
    {
        return mp_context;
    } 

    void setVSync(bool on);

    bool getVSync();

   private:
    void* mp_window  = nullptr;
    void* mp_context = nullptr;

    nbWindowEvtCallback_t m_evtCallback;
    nbWindowEvtCallback_t m_exitCallback;

    Event m_event;

    uint32_t m_windowId;
    uint32_t m_width;
    uint32_t m_height;
    float    m_aspectRatio;
    bool     m_VSyncOn       = true;
    float    m_mouseWheelPos = 0.0f;

    void _handleWindowEvents();

    void _pollEvents();

    void _calcFramerate();
};

}  // namespace nimbus