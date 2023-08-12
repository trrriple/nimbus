#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/renderer/renderer.hpp"

#include "nimbus/core/application.hpp"
#include "nimbus/renderer/renderCmdQ.hpp"
#include "nimbus/renderer/renderThread.hpp"
#include "nimbus/renderer/graphicsApi.hpp"

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>

namespace nimbus
{

RenderCmdQ* Renderer::s_renderCmdQ[k_numRenderCmdQ];
RenderCmdQ* Renderer::s_objectCmdQ[k_numObjectCmdQ];

RenderThread Renderer::s_renderThread;
uint32_t     Renderer::s_submitRenderCmdQIdx;
uint32_t     Renderer::s_processRenderCmdQIdx;
uint32_t     Renderer::s_submitObjectCmdQIdx;
uint32_t     Renderer::s_processObjectCmdQIdx;

void Renderer::s_init()
{
    // clang-format off
    static std::once_flag initFlag;
    std::call_once(initFlag,
    []()
    {
        GraphicsApi::init();

        for(int i = 0; i < k_numRenderCmdQ; i++)
        {
            s_renderCmdQ[i] = new RenderCmdQ();
        }

        s_submitRenderCmdQIdx = 0;
        s_processRenderCmdQIdx = 1;

        
        for(int i = 0; i < k_numObjectCmdQ; i++)
        {
            s_objectCmdQ[i] = new RenderCmdQ();
        }

        s_submitObjectCmdQIdx = 0;
        s_processObjectCmdQIdx = 1;

        s_renderThread.run(Renderer::_s_renderThreadFn);

    });
    // clang-format on
}

void Renderer::s_destroy()
{
    s_renderThread.stop();

    for (int i = 0; i < k_numRenderCmdQ; i++)
    {
        delete s_renderCmdQ[i];
    }
}

void Renderer::s_setScene(const glm::mat4& vpMatrix) noexcept
{
    m_vpMatrix = vpMatrix;
}

void Renderer::s_startFrame() noexcept
{
    // GraphicsApi::clear();
    s_swapAndStart();
}

void Renderer::s_endFrame() noexcept
{
    // nothing for now
}

void Renderer::s_render(ref<Shader>      p_shader,
                        ref<VertexArray> p_vertexArray,
                        int32_t          vertexCount,
                        bool             setViewProjection)
{
    NM_PROFILE();

    p_shader->bind();

    if (setViewProjection)
    {
        p_shader->setMat4("u_viewProjection", m_vpMatrix);
    }

    // do we have an index buffer?
    if (p_vertexArray->getIndexBuffer())
    {
        // we do, so drawElements
        if (vertexCount == k_detectCountIfPossible)
        {
            GraphicsApi::drawElements(p_vertexArray);
        }
        else
        {
            GraphicsApi::drawElements(p_vertexArray, vertexCount);
        }
    }
    else
    {
        // we don't so drawArrays
        if (vertexCount == k_detectCountIfPossible)
        {
            GraphicsApi::drawArrays(p_vertexArray);
        }
        else
        {
            GraphicsApi::drawArrays(p_vertexArray, vertexCount);
        }
    }
}

void Renderer::s_renderInstanced(const ref<Shader>&      p_shader,
                                 const ref<VertexArray>& p_vertexArray,
                                 int32_t                 instanceCount,
                                 int32_t                 vertexCount,
                                 bool                    setViewProjection)
{
    NM_PROFILE();

    p_shader->bind();

    if (setViewProjection)
    {
        p_shader->setMat4("u_viewProjection", m_vpMatrix);
    }

    // do we have an index buffer?
    if (p_vertexArray->getIndexBuffer())
    {
        // we do, so drawElements
        if (vertexCount == k_detectCountIfPossible)
        {
            GraphicsApi::drawElementsInstanced(p_vertexArray, instanceCount);
        }
        else
        {
            GraphicsApi::drawElementsInstanced(
                p_vertexArray, instanceCount, vertexCount);
        }
    }
    else
    {
        // we don't so drawArrays
        if (vertexCount == k_detectCountIfPossible)
        {
            GraphicsApi::drawArraysInstanced(p_vertexArray, instanceCount);
        }
        else
        {
            GraphicsApi::drawArraysInstanced(
                p_vertexArray, instanceCount, vertexCount);
        }
    }
}

void Renderer::s_swapAndStart() noexcept
{
    _s_qSwap();
    s_renderThread.setState(RenderThread::State::READY);
}

void Renderer::s_waitForRenderThread() noexcept
{
    s_renderThread.waitForState(RenderThread::State::PEND);
}

void Renderer::s_pumpCmds() noexcept
{
    uint32_t queuesToPump = std::max({k_numRenderCmdQ, k_numObjectCmdQ});

    for (uint32_t i = 0; i < queuesToPump; i++)
    {
        Renderer::s_swapAndStart();
        Renderer::s_waitForRenderThread();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////
void Renderer::_s_qSwap() noexcept
{
    s_submitRenderCmdQIdx  = (s_submitRenderCmdQIdx + 1) % k_numRenderCmdQ;
    s_processRenderCmdQIdx = (s_processRenderCmdQIdx + 1) % k_numRenderCmdQ;

    s_submitObjectCmdQIdx  = (s_submitObjectCmdQIdx + 1) % k_numObjectCmdQ;
    s_processObjectCmdQIdx = (s_processObjectCmdQIdx + 1) % k_numObjectCmdQ;
}

void Renderer::_s_renderThreadFn()
{
    SDL_GL_MakeCurrent(static_cast<SDL_Window*>(
                           Application::s_get().getWindow().getOsWindow()),
                       Application::s_get().getWindow().getContext());

    auto pendSw
        = Application::s_get().getSwBank().newSw("RenderThread Pend");

    auto processSw
        = Application::s_get().getSwBank().newSw("RenderThread Process");

    while (s_renderThread.isActive())
    {
        pendSw->split();
        s_renderThread.waitForState(RenderThread::State::READY);
        pendSw->splitAndSave();

        processSw->split();
        s_renderThread.setState(RenderThread::State::BUSY);
        // process all the commands in object queue
        _s_getProcessObjectCmdQ()->pump();
        // process all the commands in render queue
        _s_getProcessRenderCmdQ()->pump();
        s_renderThread.setState(RenderThread::State::PEND);
        processSw->splitAndSave();
    }
}

void Renderer::_s_submit(const ref<Shader>&      p_shader,
                         const ref<VertexArray>& p_vertexArray,
                         const glm::mat4&        model,
                         int32_t                 vertexCount,
                         bool                    setViewProjection)
{
    NM_PROFILE();

    p_shader->bind();
    p_shader->setMat4("u_model", model);

    if (setViewProjection)
    {
        p_shader->setMat4("u_viewProjection", m_vpMatrix);
    }

    // do we have an index buffer?
    if (p_vertexArray->getIndexBuffer())
    {
        // we do, so drawElements
        if (vertexCount == k_detectCountIfPossible)
        {
            GraphicsApi::drawElements(p_vertexArray);
        }
        else
        {
            GraphicsApi::drawElements(p_vertexArray, vertexCount);
        }
    }
    else
    {
        // we don't so drawArrays
        if (vertexCount == k_detectCountIfPossible)
        {
            GraphicsApi::drawArrays(p_vertexArray);
        }
        else
        {
            GraphicsApi::drawArrays(p_vertexArray, vertexCount);
        }
    }
}


void Renderer::_s_submitInstanced(const ref<Shader>&      p_shader,
                                  const ref<VertexArray>& p_vertexArray,
                                  int32_t                 instanceCount,
                                  const glm::mat4&        model,
                                  int32_t                 vertexCount,
                                  bool                    setViewProjection)
{
    NM_PROFILE();

    p_shader->bind();
    p_shader->setMat4("u_model", model);

    if (setViewProjection)
    {
        p_shader->setMat4("u_viewProjection", m_vpMatrix);
    }

    // do we have an index buffer?
    if (p_vertexArray->getIndexBuffer())
    {
        // we do, so drawElements
        if (vertexCount == k_detectCountIfPossible)
        {
            GraphicsApi::drawElementsInstanced(p_vertexArray, instanceCount);
        }
        else
        {
            GraphicsApi::drawElementsInstanced(
                p_vertexArray, instanceCount, vertexCount);
        }
    }
    else
    {
        // we don't so drawArrays
        if (vertexCount == k_detectCountIfPossible)
        {
            GraphicsApi::drawArraysInstanced(p_vertexArray, instanceCount);
        }
        else
        {
            GraphicsApi::drawArraysInstanced(
                p_vertexArray, instanceCount, vertexCount);
        }
    }
}

}  // namespace nimbus