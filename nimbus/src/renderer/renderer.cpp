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

RendererInternalData* Renderer::sp_data;

void Renderer::s_init()
{
    // clang-format off
    static std::once_flag initFlag;
    std::call_once(initFlag,
    []()
    {
        sp_data = new RendererInternalData();


        GraphicsApi::init();

        ////////////////////////
        // Setup and start Thread
        ////////////////////////
        for(int i = 0; i < k_numRenderCmdQ; i++)
        {
            sp_data->renderCmdQ[i] = new RenderCmdQ();
        }

        sp_data->submitRenderCmdQIdx = 0;
        sp_data->processRenderCmdQIdx = 1;

        
        for(int i = 0; i < k_numObjectCmdQ; i++)
        {
            sp_data->objectCmdQ[i] = new RenderCmdQ();
        }

        sp_data->submitObjectCmdQIdx = 0;
        sp_data->processObjectCmdQIdx = 1;

        sp_data->renderThread.run(Renderer::_s_renderThreadFn);

        ///////////////////////////
        // Make any common assets
        ///////////////////////////
        Texture::Spec texSpec;
        texSpec.width  = 1;
        texSpec.height = 1;
        sp_data->p_whiteTexture
            = Texture::s_create(Texture::Type::diffuse, texSpec);

        // being a 1x1 texture, it's only 4 bytes of data
        u32_t whiteData = 0xFFFFFFFF;
        sp_data->p_whiteTexture->setData(&whiteData, sizeof(whiteData));

        sp_data->p_blackTexture
            = Texture::s_create(Texture::Type::diffuse, texSpec);
        
        u32_t blackData = 0xFF000000;
        sp_data->p_blackTexture->setData(&blackData, sizeof(blackData));


    });
    // clang-format on
}

void Renderer::s_destroy()
{
    sp_data->p_whiteTexture = nullptr;
    sp_data->p_blackTexture = nullptr;

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

    sp_data->renderThread.stop();

    for (int i = 0; i < k_numRenderCmdQ; i++)
    {
        if (sp_data->renderCmdQ[i]->getCmdCount() != 0)
        {
            Log::coreWarn("Unprocessed commands (%i) let on queue", sp_data->renderCmdQ[i]->getCmdCount());
        }
        delete sp_data->renderCmdQ[i];
    }

    delete sp_data;
}

void Renderer::s_setScene(const glm::mat4& vpMatrix)
{
    sp_data->vpMatrix = vpMatrix;
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
    sp_data->renderThread.setState(RenderThread::State::ready);
}

void Renderer::s_waitForRenderThread()
{
    sp_data->renderThread.waitForState(RenderThread::State::pend);
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
    return sp_data->p_whiteTexture;
}

ref<Texture> Renderer::getBlackTexture()
{
    return sp_data->p_blackTexture;
}

void Renderer::s_render(ref<Shader> p_shader, ref<VertexArray> p_vertexArray, i32_t vertexCount, bool setViewProjection)
{
    NB_PROFILE();

    p_shader->bind();

    if (setViewProjection)
    {
        p_shader->setMat4("u_viewProjection", sp_data->vpMatrix);
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
        p_shader->setMat4("u_viewProjection", sp_data->vpMatrix);
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
    return sp_data->renderCmdQ[sp_data->submitRenderCmdQIdx];
}
RenderCmdQ* Renderer::_s_getProcessRenderCmdQ()
{
    return sp_data->renderCmdQ[sp_data->processRenderCmdQIdx];
}

RenderCmdQ* Renderer::_s_getSubmitObjectCmdQ()
{
    return sp_data->objectCmdQ[sp_data->submitRenderCmdQIdx];
}
RenderCmdQ* Renderer::_s_getProcessObjectCmdQ()
{
    return sp_data->objectCmdQ[sp_data->processRenderCmdQIdx];
}

void Renderer::_s_qSwap()
// Private Functions
{
    sp_data->submitRenderCmdQIdx  = (sp_data->submitRenderCmdQIdx + 1) % k_numRenderCmdQ;
    sp_data->processRenderCmdQIdx = (sp_data->processRenderCmdQIdx + 1) % k_numRenderCmdQ;

    sp_data->submitObjectCmdQIdx  = (sp_data->submitObjectCmdQIdx + 1) % k_numObjectCmdQ;
    sp_data->processObjectCmdQIdx = (sp_data->processObjectCmdQIdx + 1) % k_numObjectCmdQ;
}

void Renderer::_s_renderThreadFn()
{
    SDL_GL_MakeCurrent(static_cast<SDL_Window*>(Application::s_get().getWindow().getOsWindow()),
                       Application::s_get().getWindow().getContext());

    auto pendSw = Application::s_get().getSwBank().newSw("RenderThread Pend");

    auto processSw = Application::s_get().getSwBank().newSw("RenderThread Process");

    while (sp_data->renderThread.isActive())
    {
        pendSw->split();
        sp_data->renderThread.waitForState(RenderThread::State::ready);
        pendSw->splitAndSave();

        processSw->split();

        sp_data->renderThread.setState(RenderThread::State::busy);
        // process all the commands in render queue
        _s_getProcessRenderCmdQ()->pump();
        sp_data->renderThread.setState(RenderThread::State::pend);
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
        p_shader->setMat4("u_viewProjection", sp_data->vpMatrix);
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
        p_shader->setMat4("u_viewProjection", sp_data->vpMatrix);
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