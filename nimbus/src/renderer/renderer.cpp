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

inline static const i32_t k_numRenderCmdQ = 2;  // >= 2
inline static const i32_t k_numObjectCmdQ = 2;  // >= 2

struct RendererInternalData
{
    ///////////////////////////
    // Threads
    ///////////////////////////
    RenderThread renderThread;

    ///////////////////////////
    // Queues
    ///////////////////////////
    RenderCmdQ* renderCmdQ[k_numRenderCmdQ];
    RenderCmdQ* objectCmdQ[k_numObjectCmdQ];

    ///////////////////////////
    // State
    ///////////////////////////
    u32_t     submitRenderCmdQIdx;
    u32_t     processRenderCmdQIdx;
    u32_t     submitObjectCmdQIdx;
    u32_t     processObjectCmdQIdx;
    glm::mat4 vpMatrix = glm::mat4(1.0f);

    ///////////////////////////
    // Assets
    ///////////////////////////
    ref<Texture> p_whiteTexture;
    ref<Texture> p_blackTexture;
};

RendererInternalData* Renderer::s_data;

void Renderer::s_init()
{
    // clang-format off
    static std::once_flag initFlag;
    std::call_once(initFlag,
    []()
    {
        s_data = new RendererInternalData();


        GraphicsApi::init();

        ////////////////////////
        // Setup and start Thread
        ////////////////////////
        for(int i = 0; i < k_numRenderCmdQ; i++)
        {
            s_data->renderCmdQ[i] = new RenderCmdQ();
        }

        s_data->submitRenderCmdQIdx = 0;
        s_data->processRenderCmdQIdx = 1;

        
        for(int i = 0; i < k_numObjectCmdQ; i++)
        {
            s_data->objectCmdQ[i] = new RenderCmdQ();
        }

        s_data->submitObjectCmdQIdx = 0;
        s_data->processObjectCmdQIdx = 1;

        s_data->renderThread.run(Renderer::_s_renderThreadFn);

        ///////////////////////////
        // Make any common assets
        ///////////////////////////
        Texture::Spec texSpec;
        texSpec.width  = 1;
        texSpec.height = 1;
        s_data->p_whiteTexture
            = Texture::s_create(Texture::Type::DIFFUSE, texSpec);

        // being a 1x1 texture, it's only 4 bytes of data
        u32_t whiteData = 0xFFFFFFFF;
        s_data->p_whiteTexture->setData(&whiteData, sizeof(whiteData));

        s_data->p_blackTexture
            = Texture::s_create(Texture::Type::DIFFUSE, texSpec);
        
        u32_t blackData = 0xFF000000;
        s_data->p_blackTexture->setData(&blackData, sizeof(blackData));


    });
    // clang-format on
}

void Renderer::s_destroy()
{
    s_data->p_whiteTexture = nullptr;
    s_data->p_blackTexture = nullptr;

    // flush the queues, order matters here due to not wanting to use resources
    // that are being deleted, so we run all of the renders first before
    // doing the deletes, typically this is done the other way around where
    // objects are processed first.
    // TODO think about: is this potentially a crash point on close if objects
    // are being used that technically haven't been created yet.
    for (u32_t i = 0; i < k_numRenderCmdQ; i++)
    {
        s_swapAndStart();
        s_waitForRenderThread();
    }

    // render command queues should now be empty
    for (u32_t i = 0; i < k_numObjectCmdQ; i++)
    {
        _s_processObjectQueue();
        s_swapAndStart();
        s_waitForRenderThread();
    }

    s_data->renderThread.stop();

    for (int i = 0; i < k_numRenderCmdQ; i++)
    {
        if (s_data->renderCmdQ[i]->getCmdCount() != 0)
        {
            Log::coreWarn("Unprocessed commands (%i) let on queue", s_data->renderCmdQ[i]->getCmdCount());
        }
        delete s_data->renderCmdQ[i];
    }

    delete s_data;
}

void Renderer::s_setScene(const glm::mat4& vpMatrix)
{
    s_data->vpMatrix = vpMatrix;
}

void Renderer::s_startFrame()
{
    _s_processObjectQueue();
}

void Renderer::s_endFrame()
{
    // nothing for now
}

void Renderer::s_swapAndStart()
{
    _s_qSwap();
    s_data->renderThread.setState(RenderThread::State::READY);
}

void Renderer::s_waitForRenderThread()
{
    s_data->renderThread.waitForState(RenderThread::State::PEND);
}

void Renderer::s_pumpCmds()
{
    u32_t queuesToPump = std::max({k_numRenderCmdQ, k_numObjectCmdQ});

    for (u32_t i = 0; i < queuesToPump; i++)
    {
        _s_processObjectQueue();
        s_swapAndStart();
        s_waitForRenderThread();
    }
}

ref<Texture> Renderer::getWhiteTexture()
{
    return s_data->p_whiteTexture;
}

ref<Texture> Renderer::getBlackTexture()
{
    return s_data->p_blackTexture;
}

void Renderer::s_render(ref<Shader> p_shader, ref<VertexArray> p_vertexArray, i32_t vertexCount, bool setViewProjection)
{
    NB_PROFILE();

    p_shader->bind();

    if (setViewProjection)
    {
        p_shader->setMat4("u_viewProjection", s_data->vpMatrix);
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
                                 i32_t                   instanceCount,
                                 i32_t                   vertexCount,
                                 bool                    setViewProjection)
{
    NB_PROFILE();

    p_shader->bind();

    if (setViewProjection)
    {
        p_shader->setMat4("u_viewProjection", s_data->vpMatrix);
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
            GraphicsApi::drawElementsInstanced(p_vertexArray, instanceCount, vertexCount);
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
            GraphicsApi::drawArraysInstanced(p_vertexArray, instanceCount, vertexCount);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RenderCmdQ* Renderer::_s_getSubmitRenderCmdQ()
{
    return s_data->renderCmdQ[s_data->submitRenderCmdQIdx];
}
RenderCmdQ* Renderer::_s_getProcessRenderCmdQ()
{
    return s_data->renderCmdQ[s_data->processRenderCmdQIdx];
}

RenderCmdQ* Renderer::_s_getSubmitObjectCmdQ()
{
    return s_data->objectCmdQ[s_data->submitRenderCmdQIdx];
}
RenderCmdQ* Renderer::_s_getProcessObjectCmdQ()
{
    return s_data->objectCmdQ[s_data->processRenderCmdQIdx];
}

void Renderer::_s_qSwap()
// Private Functions
{
    s_data->submitRenderCmdQIdx  = (s_data->submitRenderCmdQIdx + 1) % k_numRenderCmdQ;
    s_data->processRenderCmdQIdx = (s_data->processRenderCmdQIdx + 1) % k_numRenderCmdQ;

    s_data->submitObjectCmdQIdx  = (s_data->submitObjectCmdQIdx + 1) % k_numObjectCmdQ;
    s_data->processObjectCmdQIdx = (s_data->processObjectCmdQIdx + 1) % k_numObjectCmdQ;
}

void Renderer::_s_renderThreadFn()
{
    SDL_GL_MakeCurrent(static_cast<SDL_Window*>(Application::s_get().getWindow().getOsWindow()),
                       Application::s_get().getWindow().getContext());

    auto pendSw = Application::s_get().getSwBank().newSw("RenderThread Pend");

    auto processSw = Application::s_get().getSwBank().newSw("RenderThread Process");

    while (s_data->renderThread.isActive())
    {
        pendSw->split();
        s_data->renderThread.waitForState(RenderThread::State::READY);
        pendSw->splitAndSave();

        processSw->split();

        s_data->renderThread.setState(RenderThread::State::BUSY);
        // process all the commands in render queue
        _s_getProcessRenderCmdQ()->pump();
        s_data->renderThread.setState(RenderThread::State::PEND);
        processSw->splitAndSave();
    }
}

void Renderer::_s_processObjectQueue()
{
    // typically we call this before starting a frame, so that all
    // object commands get procesed before render commands
    Renderer::s_submit([]() { _s_getProcessObjectCmdQ()->pump(); });
}

void Renderer::_s_submit(const ref<Shader>&      p_shader,
                         const ref<VertexArray>& p_vertexArray,
                         const glm::mat4&        model,
                         i32_t                   vertexCount,
                         bool                    setViewProjection)
{
    NB_PROFILE();

    p_shader->bind();
    p_shader->setMat4("u_model", model);

    if (setViewProjection)
    {
        p_shader->setMat4("u_viewProjection", s_data->vpMatrix);
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
                                  i32_t                   instanceCount,
                                  const glm::mat4&        model,
                                  i32_t                   vertexCount,
                                  bool                    setViewProjection)
{
    NB_PROFILE();

    p_shader->bind();
    p_shader->setMat4("u_model", model);

    if (setViewProjection)
    {
        p_shader->setMat4("u_viewProjection", s_data->vpMatrix);
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
            GraphicsApi::drawElementsInstanced(p_vertexArray, instanceCount, vertexCount);
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
            GraphicsApi::drawArraysInstanced(p_vertexArray, instanceCount, vertexCount);
        }
    }
}

}  // namespace nimbus