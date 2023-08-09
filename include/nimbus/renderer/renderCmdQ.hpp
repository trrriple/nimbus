#pragma once
#include "nimbus/core/common.hpp"

#include <mutex>

namespace nimbus
{

class RenderCmdQ
{
    const uint32_t k_cmdBufSize = (2 << 23);

   public:
    typedef void (*renderCmdFn)(void*);

    RenderCmdQ();
    ~RenderCmdQ();

    void* slot(renderCmdFn fn, uint32_t size);
    void  slotDone();

    uint32_t getCmdCount()
    {
        // should be called with mutual exclusion
        return m_cmdCount;
    }

    void prepTmpQ();
    void processTmpQ();

   private:
    uint8_t*                 mp_cmdBuf;
    uint8_t*                 mp_cmdBufPtr;
    uint32_t                 m_cmdCount = 0;
    uint8_t*                 mp_cmdBufTmp;
    uint32_t                 m_cmdCountTmp  = 0;
    uint32_t                 m_cmdBufUsedSz = 0;
};

}  // namespace nimbus