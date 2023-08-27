#pragma once

#include "nimbus/core/common.hpp"
#include "nimbus/renderer/buffer.hpp"
#include "nimbus/renderer/shader.hpp"
#include "nimbus/renderer/renderCmdQ.hpp"
#include "nimbus/renderer/renderThread.hpp"
#include "nimbus/renderer/texture.hpp"

#include "glm.hpp"

#include <thread>
#include <mutex>
#include <condition_variable>

namespace nimbus
{

struct RendererInternalData;

class Renderer : public refCounted
{
    ////////////////////////////////////////////////////////////////////////////
    // Variables
    ////////////////////////////////////////////////////////////////////////////
   public:
    inline static const i32_t k_detectCountIfPossible = -1;

    static void s_init();
    static void s_destroy();

   private:
    static RendererInternalData* sp_data;

    ////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////
   public:
    template <typename T>
    inline static void s_submit(T&& func)
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
    inline static void s_submitObject(T&& func)
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
    static void s_setScene(const glm::mat4& vpMatrix);

    static void s_startFrame();

    static void s_endFrame();

    static void s_swapAndStart();

    static void s_waitForRenderThread();

    static void s_pumpCmds();

    static ref<Texture> getWhiteTexture();

    static ref<Texture> getBlackTexture();

    static void s_render(ref<Shader>      p_shader,
                         ref<VertexArray> p_vertexArray,
                         i32_t            vertexCount       = k_detectCountIfPossible,
                         bool             setViewProjection = true);

    static void s_renderInstanced(const ref<Shader>&      p_shader,
                                  const ref<VertexArray>& p_vertexArray,
                                  i32_t                   instanceCount,
                                  i32_t                   vertexCount       = k_detectCountIfPossible,
                                  bool                    setViewProjection = true);

   private:
    static RenderCmdQ* _s_getSubmitRenderCmdQ();

    static RenderCmdQ* _s_getProcessRenderCmdQ();

    static RenderCmdQ* _s_getSubmitObjectCmdQ();

    static RenderCmdQ* _s_getProcessObjectCmdQ();

    static void _s_qSwap();

    static void _s_renderThreadFn();

    static void _s_processObjectQueue();

    ///////////////////////////
    // TODO port these
    ///////////////////////////
    static void _s_submit(const ref<Shader>&      p_shader,
                          const ref<VertexArray>& p_vertexArray,
                          const glm::mat4&        model,
                          i32_t                   vertexCount       = k_detectCountIfPossible,
                          bool                    setViewProjection = true);

    static void _s_submitInstanced(const ref<Shader>&      p_shader,
                                   const ref<VertexArray>& p_vertexArray,
                                   i32_t                   instanceCount,
                                   const glm::mat4&        model,
                                   i32_t                   vertexCount       = k_detectCountIfPossible,
                                   bool                    setViewProjection = true);

    friend class RenderThread;
};
}  // namespace nimbus