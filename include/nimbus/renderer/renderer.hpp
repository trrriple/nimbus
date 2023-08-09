#pragma once

#include "nimbus/core/common.hpp"
#include "nimbus/renderer/buffer.hpp"
#include "nimbus/renderer/shader.hpp"
#include "nimbus/renderer/renderCmdQ.hpp"

#include "glm.hpp"

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace nimbus
{
class Renderer : public refCounted
{
   public:
    inline static const int32_t k_detectCountIfPossible = -1;

    static void s_init(void* p_window, void* p_context);
    static void s_destroy();

    static void s_setScene(const glm::mat4& vpMatrix);

    template <typename T>
    static void s_submit(T&& func)
    {
        auto renderCmd = [](void* ptr)
        {
            auto pFunc = (T*)ptr;
            (*pFunc)();
            ;
            pFunc->~T();
        };

        std::lock_guard<std::mutex> lock(s_cmdQMutex);
        auto storageBuffer = s_cmdQ.slot(renderCmd, sizeof(func));
        new (storageBuffer) T(std::forward<T>(func));
        s_cmdQCondition.notify_one();
    }

    // static void s_submit(std::function<void()> fn);

    static void s_processHook();

    static void s_render(ref<Shader>                p_shader,
                         ref<VertexArray>           p_vertexArray,
                         int32_t vertexCount       = k_detectCountIfPossible,
                         bool    setViewProjection = true);

    static void s_renderInstanced(const ref<Shader>&      p_shader,
                                  const ref<VertexArray>& p_vertexArray,
                                  int32_t                 instanceCount,
                                  int32_t vertexCount = k_detectCountIfPossible,
                                  bool    setViewProjection = true);

   private:
    static RenderCmdQ              s_cmdQ;
    static std::thread             s_renderThread;
    static std::mutex              s_cmdQMutex;
    static std::condition_variable s_cmdQCondition;
    static bool                    s_terminate;
    static bool                    s_initialized;

    inline static glm::mat4 mp_vpMatrix = glm::mat4(1.0f);

    static void _s_serviceQueue(void* p_window, void* p_context);
    // static void _s_processCmd(const Command& cmd);

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

};
}  // namespace nimbus