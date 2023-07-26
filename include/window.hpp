#pragma once

#include "event.hpp"
#include "keyCode.hpp"

#include <functional>
#include <cstdint>
#include <string>

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

    bool keyPressed(ScanCode ScanCode) const;

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
    void* mp_window = nullptr;
    void* mp_context = nullptr;

    nbWindowEvtCallback_t m_evtCallback;
    nbWindowEvtCallback_t m_exitCallback;

    Event m_event;

    uint32_t m_sdlWindowId;
    uint32_t m_width;
    uint32_t m_height;
    float    m_aspectRatio;
    bool     m_VSyncOn = true;

    void _handleWindowEvents();

    void _pollEvents();

    void _calcFramerate();

};

}  // namespace nimbus