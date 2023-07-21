#pragma once

#include "camera.hpp"
#include "common.hpp"
#include "glm.hpp"
#include "platform/buffer.hpp"
#include "renderer/shader.hpp"

namespace nimbus
{
class Renderer
{  
    
   public:
    inline static const int32_t k_detectCountIfPossible = -1;
    
    static void init();
    static void destroy();

    static void setScene(Camera& camera);

    static void submit(const ref<Shader>&      p_shader,
                       const ref<VertexArray>& p_vertexArray,
                       const glm::mat4&        p_model,
                       int32_t vertexCount       = k_detectCountIfPossible,
                       bool    setViewProjection = true);

    static void submit(const ref<Shader>&      p_shader,
                       const ref<VertexArray>& p_vertexArray,
                       int32_t vertexCount = k_detectCountIfPossible,
                       bool    setViewProjection = true);

    static void submitInstanced(const ref<Shader>&      p_shader,
                                const ref<VertexArray>& p_vertexArray,
                                int32_t                 instanceCount,
                                const glm::mat4&        p_model,
                                int32_t vertexCount = k_detectCountIfPossible,
                                bool    setViewProjection = true);

    static void submitInstanced(const ref<Shader>&      p_shader,
                                const ref<VertexArray>& p_vertexArray,
                                int32_t                 instanceCount,
                                int32_t vertexCount = k_detectCountIfPossible,
                                bool    setViewProjection = true);

   private:
    inline static glm::mat4* mp_vpMatrix = nullptr;
};
}  // namespace nimbus