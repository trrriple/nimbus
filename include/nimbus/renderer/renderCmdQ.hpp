#pragma once
#include "nimbus/core/common.hpp"

#include <mutex>

namespace nimbus
{

class RenderCmdQ
{
    const u32_t k_cmdBufSize = (2 << 16);

   public:
    typedef void (*renderCmdFn)(void*);

    RenderCmdQ();
    ~RenderCmdQ();

    void* slot(renderCmdFn fn, u32_t size);

    inline u32_t getCmdCount()
    {
        return m_cmdCount;
    }

    void pump();

   private:
    u8_t* mp_cmdBuf;
    u8_t* mp_cmdBufPtr;
    u32_t m_cmdCount     = 0;
    u32_t m_cmdBufUsedSz = 0;
};

}  // namespace nimbus