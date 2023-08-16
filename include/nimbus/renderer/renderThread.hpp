#pragma once
#include "nimbus/core/common.hpp"

#include <thread>
#include <mutex>
#include <condition_variable>

namespace nimbus {

class RenderThread
{
   public:
    enum class State
    {
        PEND = 0,
        BUSY,
        READY,
        DEAD
    };

    RenderThread();
    ~RenderThread();

    void run(void (*fn)());
    void waitForState(State state);
    void setState(State state);
    void stop();

    bool isActive() const 
    {
        return m_active;
    }

   private:
    std::thread m_thread;

    State m_state;
    bool  m_active;

    std::mutex              m_pendMtx;
    std::condition_variable m_pendCond;
};

}  // namespace nimbus