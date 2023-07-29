#pragma once

#include "nimbus/renderer/graphicsApi.hpp"

namespace nimbus
{
class GlGraphicsApi : public GraphicsApi
{
   public:
    static void init();
    static void clear();

    static void drawElements(const ref<VertexArray>& p_vertexArray,
                             uint32_t                vertexCount = 0);

    static void drawArrays(const ref<VertexArray>& p_vertexArray,
                           uint32_t                vertexCount = 0);

    static void drawElementsInstanced(const ref<VertexArray>& p_vertexArray,
                                      uint32_t                instanceCount,
                                      uint32_t                vertexCount = 0);

    static void drawArraysInstanced(const ref<VertexArray>& p_vertexArray,
                                    uint32_t                instanceCount,
                                    uint32_t                vertexCount = 0);

    static void setViewportSize(int x, int y, int w, int h);

    static void setWireframe(bool on);

    static void setDepthTest(bool on);

    static void setBlendingMode(GraphicsApi::BlendingMode);

   private:
    static void _enableGlErrPrint();
};
}  // namespace nimbus