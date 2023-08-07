#pragma once

#include "nimbus/core/common.hpp"
#include "nimbus/renderer/buffer.hpp"
#include "nimbus/renderer/shader.hpp"

#include "glm.hpp"

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace nimbus
{
class Renderer
{
   public:
    typedef void(*renderCmdFn)(void*);


    enum class CmdType
    {
        DRAW_STANDARD,
        DRAW_INSTANCED,
        SET_SHADER,
        SET_TEXTURE,
        // ... other command types
    };

    struct CmdBase
    {
        CmdType type;
    };

    struct DrawStandardCmd : CmdBase
    {
        ref<Shader>      p_shader          = nullptr;
        ref<VertexArray> p_vertexArray     = nullptr;
        glm::mat4        model             = glm::mat4(1.0f);
        int32_t          vertexCount       = k_detectCountIfPossible;
        int32_t          instanceCount     = 1;
        bool             setViewProjection = true;
        // ... other draw-specific parameters
    };

    // struct SetShaderCommand : CommandBase
    // {
    //     GLuint shaderID;
    //     // ... other shader-specific parameters
    // };

    // struct SetTextureCommand : CommandBase
    // {
    //     GLuint textureID;
    //     int    slot;
    //     // ... other texture-specific parameters
    // };



    inline static const int32_t k_detectCountIfPossible = -1;

    static void s_init(void* p_window, void* p_context);
    static void s_destroy();

    static void s_setScene(const glm::mat4& vpMatrix);

    static void s_submit(std::function<void()> fn);

    struct VertexBufferInfo
    {
        uint32_t id;
        void*    p_memory;
    };

    static VertexBufferInfo s_createVbo(const void* verticies,
                                        uint32_t    size,
                                        uint32_t    flags);

   private:
    inline static glm::mat4 mp_vpMatrix = glm::mat4(1.0f);

    static std::thread                       s_renderThread;
    static std::queue<std::function<void()>> s_cmdQueue;
    static std::mutex                        s_queueMutex;
    static std::condition_variable           s_cmdCondition;
    static bool                              s_terminate;
    static bool                    s_initialized;

    static void _s_serviceQueue(void* p_window, void* p_context);
    // static void _s_processCmd(const Command& cmd);

    static void _s_submit(const ref<Shader>&      p_shader,
                          const ref<VertexArray>& p_vertexArray,
                          const glm::mat4&        model,
                          int32_t vertexCount       = k_detectCountIfPossible,
                          bool    setViewProjection = true);

    static void _s_submit(const ref<Shader>&      p_shader,
                          const ref<VertexArray>& p_vertexArray,
                          int32_t vertexCount       = k_detectCountIfPossible,
                          bool    setViewProjection = true);

    static void _s_submitInstanced(const ref<Shader>&      p_shader,
                                   const ref<VertexArray>& p_vertexArray,
                                   int32_t                 instanceCount,
                                   const glm::mat4&        model,
                                   int32_t                 vertexCount
                                   = k_detectCountIfPossible,
                                   bool setViewProjection = true);

    static void _s_submitInstanced(const ref<Shader>&      p_shader,
                                   const ref<VertexArray>& p_vertexArray,
                                   int32_t                 instanceCount,
                                   int32_t                 vertexCount
                                   = k_detectCountIfPossible,
                                   bool setViewProjection = true);
};
}  // namespace nimbus