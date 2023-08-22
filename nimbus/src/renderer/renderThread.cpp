#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/core/application.hpp"
#include "nimbus/renderer/renderThread.hpp"
#include "nimbus/renderer/renderer.hpp"

#include <thread>

namespace nimbus
{

RenderThread::RenderThread()
{
    m_state  = State::dead;
    m_active = false;
}

RenderThread::~RenderThread()
{
}
void RenderThread::run(void (*fn)())
{
    SDL_GL_MakeCurrent(static_cast<SDL_Window*>(Application::s_get().getWindow().getOsWindow()), nullptr);
    m_active = true;
    m_state  = State::busy;
    m_thread = std::thread(fn);
}

void RenderThread::waitForState(State state)
{
    std::unique_lock<std::mutex> lock(m_pendMtx);

    // clang-format off
    m_pendCond.wait(lock, [=]() { 
        
        if(m_state == state || !m_active)
        {
            return true;
        }
        else
        {
            return false;
        }
    
    });
    // clang-format on
}

void RenderThread::setState(State state)
{
    std::lock_guard<std::mutex> lock(m_pendMtx);
    m_state = state;
    m_pendCond.notify_all();
}

void RenderThread::stop()
{
    m_active = false;
    setState(State::dead);
    m_thread.join();
}

}  // namespace nimbus