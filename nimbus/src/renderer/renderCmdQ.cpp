#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/renderer/renderCmdQ.hpp"

namespace nimbus
{

RenderCmdQ::RenderCmdQ()
{
    mp_cmdBuf = static_cast<u8_t*>(calloc(k_cmdBufSize, sizeof(u8_t)));
    NB_CORE_ASSERT(mp_cmdBuf, "Failed to allocate command queue!");
    mp_cmdBufPtr = mp_cmdBuf;
}

RenderCmdQ::~RenderCmdQ()
{
    free(mp_cmdBuf);
}

void* RenderCmdQ::slot(renderCmdFn fn, u32_t size)
{
    *reinterpret_cast<renderCmdFn*>(mp_cmdBufPtr) = fn;
    mp_cmdBufPtr += sizeof(renderCmdFn);
    m_cmdBufUsedSz += sizeof(renderCmdFn);

    *reinterpret_cast<u32_t*>(mp_cmdBufPtr) = size;
    mp_cmdBufPtr += sizeof(u32_t);
    m_cmdBufUsedSz += sizeof(u32_t);

    void* slot = mp_cmdBufPtr;
    mp_cmdBufPtr += size;
    m_cmdBufUsedSz += size;

    NB_CORE_ASSERT(m_cmdBufUsedSz < k_cmdBufSize, "Command Queue Overflow!");

    m_cmdCount++;
    return slot;
}

void RenderCmdQ::pump()
{
    // exit if there's nothing to process
    if (m_cmdCount > 0)
    {
        u8_t* ptr = mp_cmdBuf;

        for (u32_t i = 0; i < m_cmdCount; i++)
        {
            // grab the function out
            renderCmdFn fn = *reinterpret_cast<renderCmdFn*>(ptr);
            ptr += sizeof(renderCmdFn);

            // grab the size of the data
            u32_t size = *reinterpret_cast<u32_t*>(ptr);
            ptr += sizeof(u32_t);

            // execute the function with the data
            fn(ptr);

            ptr += size;
        }

        mp_cmdBufPtr   = mp_cmdBuf;
        m_cmdCount     = 0;
        m_cmdBufUsedSz = 0;
    }
}

}  // namespace nimbus