#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/renderer/renderer.hpp"
#include "nimbus/renderer/graphicsApi.hpp"

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>

namespace nimbus
{

std::thread                       Renderer::s_renderThread;
std::mutex                        Renderer::s_queueMutex;
std::condition_variable           Renderer::s_cmdCondition;
std::queue<std::function<void()>> Renderer::s_cmdQueue;
bool                              Renderer::s_terminate   = false;
bool                              Renderer::s_initialized = false;

void Renderer::s_init(void* p_window, void* p_context)
{
    // clang-format off
    static std::once_flag initFlag;
    std::call_once(initFlag,
    [p_window, p_context]()
    {
        s_initialized = true;

        s_renderThread = std::thread(&Renderer::_s_serviceQueue, p_window, p_context);
    });
    // clang-format on
}

void Renderer::s_destroy()
{
    {
        std::lock_guard<std::mutex> lock(s_queueMutex);
        s_terminate = true;
    }
    s_cmdCondition.notify_one();
    s_renderThread.join();
}

void Renderer::s_setScene(const glm::mat4& vpMatrix)
{
    mp_vpMatrix = vpMatrix;
}

void Renderer::s_submit(std::function<void()> fn)
{
    std::lock_guard<std::mutex> lock(s_queueMutex);
    s_cmdQueue.push(fn);
    s_cmdCondition.notify_one();
}

void Renderer::s_processHook()
{
    static std::queue<std::function<void()>> localQueue;

    {
        std::unique_lock<std::mutex> lock(s_queueMutex);
        s_cmdCondition.wait(
            lock, []() { return !s_cmdQueue.empty() || s_terminate; });

        // Transfer commands to the local queue
        while (!s_cmdQueue.empty())
        {
            localQueue.push(std::move(s_cmdQueue.front()));
            s_cmdQueue.pop();
        }
    }  // Mutex is released here

    // Process the commands outside of the locked section
    while (!localQueue.empty())
    {
        auto& command = localQueue.front();
        command();
        localQueue.pop();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////
void Renderer::_s_serviceQueue(void* p_window, void* p_context)
{
    
    SDL_GL_MakeCurrent(static_cast<SDL_Window*>(p_window), p_context);

    std::queue<std::function<void()>> localQueue;
    
    while (!s_terminate)
    {

        {
            std::unique_lock<std::mutex> lock(s_queueMutex);
            s_cmdCondition.wait(
                lock, []() { return !s_cmdQueue.empty() || s_terminate; });

            // Transfer commands to the local queue
            while (!s_cmdQueue.empty())
            {
                localQueue.push(std::move(s_cmdQueue.front()));
                s_cmdQueue.pop();
            }
        }  // Mutex is released here

        // Process the commands outside of the locked section
        while (!localQueue.empty())
        {
            auto& command = localQueue.front();
            command();
            localQueue.pop();
        }
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
        p_shader->setMat4("u_viewProjection", mp_vpMatrix);
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

void Renderer::s_render(const ref<Shader>&      p_shader,
                        const ref<VertexArray>& p_vertexArray,
                        int32_t                 vertexCount,
                        bool                    setViewProjection)
{
    NM_PROFILE();

    p_shader->bind();

    if (setViewProjection)
    {
        p_shader->setMat4("u_viewProjection", mp_vpMatrix);
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
        p_shader->setMat4("u_viewProjection", mp_vpMatrix);
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

void Renderer::_s_submitInstanced(const ref<Shader>&      p_shader,
                                  const ref<VertexArray>& p_vertexArray,
                                  int32_t                 instanceCount,
                                  int32_t                 vertexCount,
                                  bool                    setViewProjection)
{
    NM_PROFILE();

    p_shader->bind();

    if (setViewProjection)
    {
        p_shader->setMat4("u_viewProjection", mp_vpMatrix);
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