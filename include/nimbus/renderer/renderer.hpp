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

    static void s_init() noexcept;
    static void s_destroy() noexcept;

   private:
    inline static glm::mat4     m_vpMatrix      = glm::mat4(1.0f);
    inline static const int32_t k_numRenderCmdQ = 2;  // >= 2
    inline static const int32_t k_numObjectCmdQ = 2;  // >= 2
    static RenderCmdQ*          s_renderCmdQ[k_numRenderCmdQ];
    static RenderCmdQ*          s_objectCmdQ[k_numRenderCmdQ];
    static uint32_t             s_submitRenderCmdQIdx;
    static uint32_t             s_processRenderCmdQIdx;
    static uint32_t             s_submitObjectCmdQIdx;
    static uint32_t             s_processObjectCmdQIdx;

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

            // destruct any captured variables
            pFunc->~T();
        };

        auto slot = _s_getSubmitRenderCmdQ()->slot(renderCmd, sizeof(func));
        new (slot) T(std::forward<T>(func));
    }

    template <typename T>
    inline static void s_submitObject(T&& func) noexcept
    {
        auto objectCmd = [](void* ptr)
        {
            auto pFunc = (T*)ptr;
            (*pFunc)();

            // destruct any captured variables
            pFunc->~T();
        };
        auto slot = _s_getSubmitObjectCmdQ()->slot(objectCmd, sizeof(func));
        new (slot) T(std::forward<T>(func));
    }

    // TODO consider this
    static void s_setScene(const glm::mat4& vpMatrix) noexcept;

    static void s_startFrame()noexcept;

    static void s_endFrame()noexcept;

    static void s_swapAndStart()noexcept;

    static void s_waitForRenderThread() noexcept;

    static void s_pumpCmds() noexcept;

    static void s_render(ref<Shader>      p_shader,
                         ref<VertexArray> p_vertexArray,
                         int32_t          vertexCount = k_detectCountIfPossible,
                         bool             setViewProjection = true) noexcept;

    static void s_renderInstanced(const ref<Shader>&      p_shader,
                                  const ref<VertexArray>& p_vertexArray,
                                  int32_t                 instanceCount,
                                  int32_t vertexCount = k_detectCountIfPossible,
                                  bool    setViewProjection = true) noexcept;

   private:
    inline static RenderCmdQ* _s_getSubmitRenderCmdQ() noexcept
    {
        return s_renderCmdQ[s_submitRenderCmdQIdx];
    }
    inline static RenderCmdQ* _s_getProcessRenderCmdQ() noexcept
    {
        return s_renderCmdQ[s_processRenderCmdQIdx];
    }

    inline static RenderCmdQ* _s_getSubmitObjectCmdQ() noexcept
    {
        return s_objectCmdQ[s_submitRenderCmdQIdx];
    }
    inline static RenderCmdQ* _s_getProcessObjectCmdQ() noexcept
    {
        return s_objectCmdQ[s_processRenderCmdQIdx];
    }

    static void _s_qSwap() noexcept;

    static void _s_renderThreadFn() noexcept;

    static void _s_processObjectQueue() noexcept;

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