#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/renderer/renderCmdQ.hpp"



namespace nimbus
{

RenderCmdQ::RenderCmdQ()
{
    mp_cmdBuf = static_cast<uint8_t*>(calloc(k_cmdBufSize, sizeof(uint8_t)));
    NM_CORE_ASSERT(mp_cmdBuf, "Failed to allocate command queue!");
    mp_cmdBufPtr = mp_cmdBuf;

    mp_cmdBufTmp = static_cast<uint8_t*>(calloc(k_cmdBufSize, sizeof(uint8_t)));
    NM_CORE_ASSERT(mp_cmdBuf, "Failed to Temp command queue!");
}

RenderCmdQ::~RenderCmdQ()
{
    free(mp_cmdBuf);
    free(mp_cmdBufTmp);
}

void* RenderCmdQ::slot(renderCmdFn fn, uint32_t size)
{
    // expect this function to be called with mutual exclusion
    // TODO: alignment
    *(renderCmdFn*)mp_cmdBufPtr = fn;
    mp_cmdBufPtr += sizeof(renderCmdFn);
    m_cmdBufUsedSz += sizeof(renderCmdFn);

    *(uint32_t*)mp_cmdBufPtr = size;
    mp_cmdBufPtr += sizeof(uint32_t);
    m_cmdBufUsedSz +=  sizeof(uint32_t);

    void* slot = mp_cmdBufPtr;
    mp_cmdBufPtr += size;
    m_cmdBufUsedSz += size;

    NM_CORE_ASSERT(m_cmdBufUsedSz < k_cmdBufSize, "Command Queue Overflow!");

    m_cmdCount++;
    return slot;
}

void RenderCmdQ::prepTmpQ()
{
    // expect this function to be called with mutual exclusion
    
    memcpy(mp_cmdBufTmp, mp_cmdBuf, m_cmdBufUsedSz);
    m_cmdCountTmp = m_cmdCount;

    // reset the command buffer
    mp_cmdBufPtr   = mp_cmdBuf;
    m_cmdCount     = 0;
    m_cmdBufUsedSz = 0;
}

void RenderCmdQ::processTmpQ()
{
    for (uint32_t i = 0; i < m_cmdCountTmp; i++)
    {
        // grab the function out
        renderCmdFn fn = *(renderCmdFn*)mp_cmdBufTmp;
        mp_cmdBufTmp += sizeof(renderCmdFn);

        // grab the size of the data
        uint32_t size = *(uint32_t*)mp_cmdBufTmp;
        mp_cmdBufTmp += sizeof(uint32_t);

        // execute the function with the data
        fn(mp_cmdBufTmp);

        mp_cmdBufTmp += size;
    }
}

}  // namespace nimbus