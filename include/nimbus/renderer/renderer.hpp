#pragma once

#include "nimbus/common.hpp"
#include "nimbus/camera.hpp"
#include "nimbus/renderer/buffer.hpp"
#include "nimbus/renderer/shader.hpp"

#include "glm.hpp"

namespace nimbus
{
class Renderer
{
   public:
    inline static const int32_t k_detectCountIfPossible = -1;

    static void s_init();
    static void s_destroy();

    static void s_setScene(Camera& camera);

    static void s_submit(const ref<Shader>&      p_shader,
                         const ref<VertexArray>& p_vertexArray,
                         const glm::mat4&        p_model,
                         int32_t vertexCount       = k_detectCountIfPossible,
                         bool    setViewProjection = true);

    static void s_submit(const ref<Shader>&      p_shader,
                         const ref<VertexArray>& p_vertexArray,
                         int32_t vertexCount       = k_detectCountIfPossible,
                         bool    setViewProjection = true);

    static void s_submitInstanced(const ref<Shader>&      p_shader,
                                  const ref<VertexArray>& p_vertexArray,
                                  int32_t                 instanceCount,
                                  const glm::mat4&        p_model,
                                  int32_t vertexCount = k_detectCountIfPossible,
                                  bool    setViewProjection = true);

    static void s_submitInstanced(const ref<Shader>&      p_shader,
                                  const ref<VertexArray>& p_vertexArray,
                                  int32_t                 instanceCount,
                                  int32_t vertexCount = k_detectCountIfPossible,
                                  bool    setViewProjection = true);

   private:
    inline static glm::mat4* mp_vpMatrix = nullptr;
};
}  // namespace nimbus