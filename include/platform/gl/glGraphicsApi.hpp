#pragma once

#include "nimbus/renderer/graphicsApi.hpp"

namespace nimbus
{
class GlGraphicsApi : public GraphicsApi
{
   public:
    static void init() noexcept;

    static void clear() noexcept;

    static void clearColor(glm::vec4 color) noexcept;

    static void drawElements(ref<VertexArray> p_vertexArray,
                             uint32_t         vertexCount = 0) noexcept;

    static void drawArrays(ref<VertexArray> p_vertexArray,
                           uint32_t         vertexCount = 0) noexcept;

    static void drawElementsInstanced(ref<VertexArray> p_vertexArray,
                                      uint32_t         instanceCount,
                                      uint32_t vertexCount = 0) noexcept;

    static void drawArraysInstanced(ref<VertexArray> p_vertexArray,
                                    uint32_t         instanceCount,
                                    uint32_t         vertexCount = 0) noexcept;

    static void setViewportSize(int x, int y, int w, int h) noexcept;

    static void setWireframe(bool on) noexcept;

    static void setDepthTest(bool on) noexcept;

    static void setBlendingMode(GraphicsApi::BlendingMode) noexcept;

   private:
    static void _enableGlErrPrint() noexcept;
};
}  // namespace nimbus