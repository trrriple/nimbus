#pragma once

#include "event.hpp"
#include "SDL.h"

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

    bool keyPressed(uint32_t keyCode);

    uint32_t getHeight() const
    {
        return m_height;
    }
    uint32_t getWidth() const
    {
        return m_width;
    }

    SDL_Window* getSDLWindow() const
    {
        return mp_window;
    }

    SDL_GLContext getContext() const
    {
        return m_context;
    }

    void setVSync(bool on);

    bool getVSync();

   private:
    SDL_Window*           mp_window = nullptr;
    SDL_GLContext         m_context = nullptr;
    
    nbWindowEvtCallback_t m_evtCallback;
    nbWindowEvtCallback_t m_exitCallback;
    
    Event                 m_event;

    uint32_t m_sdlWindowId;
    uint32_t m_width;
    uint32_t m_height;
    bool     m_VSyncOn = true;

    void _handleWindowEvents();

    void _pollEvents();

    void _calcFramerate();

};

}  // namespace nimbus