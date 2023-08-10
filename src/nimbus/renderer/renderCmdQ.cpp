#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/renderer/renderCmdQ.hpp"



namespace nimbus
{

RenderCmdQ::RenderCmdQ() noexcept
{
    mp_cmdBuf = static_cast<uint8_t*>(calloc(k_cmdBufSize, sizeof(uint8_t)));
    NM_CORE_ASSERT(mp_cmdBuf, "Failed to allocate command queue!");
    mp_cmdBufPtr = mp_cmdBuf;
}

RenderCmdQ::~RenderCmdQ() noexcept
{
    free(mp_cmdBuf);
}

void* RenderCmdQ::slot(renderCmdFn fn, uint32_t size) noexcept
{
    // expect this function to be called with mutual exclusion
    *(renderCmdFn*)mp_cmdBufPtr = fn;
    mp_cmdBufPtr += sizeof(renderCmdFn);
    m_cmdBufUsedSz += sizeof(renderCmdFn);

    *(uint32_t*)mp_cmdBufPtr = size;
    mp_cmdBufPtr += sizeof(uint32_t);
    m_cmdBufUsedSz += sizeof(uint32_t);

    void* slot = mp_cmdBufPtr;
    mp_cmdBufPtr += size;
    m_cmdBufUsedSz += size;

    NM_CORE_ASSERT(m_cmdBufUsedSz < k_cmdBufSize, "Command Queue Overflow!");

    m_cmdCount++;
    return slot;
}

void RenderCmdQ::pump() noexcept
{
    // exit if there's nothing to process
    if (m_cmdCount > 0)
    {
        uint8_t* ptr = mp_cmdBuf;

        for (uint32_t i = 0; i < m_cmdCount; i++)
        {
            // grab the function out
            renderCmdFn fn = *(renderCmdFn*)ptr;
            ptr += sizeof(renderCmdFn);

            // grab the size of the data
            uint32_t size = *(uint32_t*)ptr;
            ptr += sizeof(uint32_t);

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