#pragma once

#include "nimbus/core/common.hpp"
#include "nimbus/renderer/buffer.hpp"
#include "nimbus/renderer/shader.hpp"
#include "nimbus/renderer/renderCmdQ.hpp"
#include "nimbus/renderer/renderThread.hpp"

#include "glm.hpp"

#include <thread>
#include <mutex>
#include <condition_variable>

namespace nimbus
{
class Renderer : public refCounted
{
    ////////////////////////////////////////////////////////////////////////////
    // Variables
    ////////////////////////////////////////////////////////////////////////////
   public:
    inline static const int32_t k_detectCountIfPossible = -1;

    static void s_init();
    static void s_destroy();


   private:
    inline static glm::mat4     m_vpMatrix      = glm::mat4(1.0f);
    inline static const int32_t k_numRenderCmdQ = 2; // >= 2
    static RenderCmdQ*          s_cmdQ[k_numRenderCmdQ];
    static uint32_t             s_submitCmdQIdx;
    static uint32_t             s_renderCmdQIdx;

    ///////////////////////////
    // Threads
    ///////////////////////////
    static RenderThread s_renderThread;

    ////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////
   public:
    template <typename T>
    inline static void s_submit(T&& func) noexcept
    {
        auto renderCmd = [](void* ptr)
        {
            auto pFunc = (T*)ptr;
            (*pFunc)();
            ;
            pFunc->~T();
        };
        auto storageBuffer = _s_getSubmitCmdQ()->slot(renderCmd, sizeof(func));
        new (storageBuffer) T(std::forward<T>(func));
    }

    // TODO consider this
    static void s_setScene(const glm::mat4& vpMatrix);

    static void s_startFrame();

    static void s_endFrame();

    static void s_swapAndStart();

    static void s_waitForRenderThread();

    static void s_render(ref<Shader>      p_shader,
                         ref<VertexArray> p_vertexArray,
                         int32_t          vertexCount = k_detectCountIfPossible,
                         bool             setViewProjection = true);

    static void s_renderInstanced(const ref<Shader>&      p_shader,
                                  const ref<VertexArray>& p_vertexArray,
                                  int32_t                 instanceCount,
                                  int32_t vertexCount = k_detectCountIfPossible,
                                  bool    setViewProjection = true);

   private:
    inline static RenderCmdQ* _s_getSubmitCmdQ() noexcept
    {
        return s_cmdQ[s_submitCmdQIdx];
    }
    inline static RenderCmdQ* _s_getRenderCmdQ() noexcept
    {
        return s_cmdQ[s_renderCmdQIdx];
    }

    static void _s_qSwap() noexcept;

    static void _s_renderThreadFn();


    ///////////////////////////
    // TODO port these
    ///////////////////////////
    static void _s_submit(const ref<Shader>&      p_shader,
                          const ref<VertexArray>& p_vertexArray,
                          const glm::mat4&        model,
                          int32_t vertexCount       = k_detectCountIfPossible,
                          bool    setViewProjection = true);

    static void _s_submitInstanced(const ref<Shader>&      p_shader,
                                   const ref<VertexArray>& p_vertexArray,
                                   int32_t                 instanceCount,
                                   const glm::mat4&        model,
                                   int32_t                 vertexCount
                                   = k_detectCountIfPossible,
                                   bool setViewProjection = true);



    friend class RenderThread;
};
}  // namespace nimbus