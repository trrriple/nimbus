#pragma once

#include "nimbus/renderer/buffer.hpp"

namespace nimbus
{
class NIMBUS_API GraphicsApi
{
   public:
    enum class BlendingMode
    {
        additive = 0,           // GL_ONE, GL_ONE
        subtract,               // GL_ZERO, GL_ONE_MINUS_SRC_COLOR
        multiply,               // GL_DST_COLOR, GL_ZERO
        screen,                 // GL_ONE, GL_ONE_MINUS_SRC_COLOR
        replace,                // GL_ONE, GL_ZERO
        alphaBlend,             // GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA
        alphaPremultiplied,     // GL_ONE, GL_ONE_MINUS_SRC_ALPHA
        sourceAlphaAdditive,    // GL_SRC_ALPHA, GL_ONE
    };

    static void init();

    static void clear();

    static void clearColor(glm::vec4 color);

    static void drawElements(ref<VertexArray> p_vertexArray, u32_t vertexCount = 0);

    static void drawArrays(ref<VertexArray> p_vertexArray, u32_t vertexCount = 0);

    static void drawElementsInstanced(ref<VertexArray> p_vertexArray, u32_t instanceCount, u32_t vertexCount = 0);

    static void drawArraysInstanced(ref<VertexArray> p_vertexArray, u32_t instanceCount, u32_t vertexCount = 0);

    static void setViewportSize(int x, int y, int w, int h);

    static void setWireframe(bool on);

    inline static bool getWireframe()
    {
        return s_wireframe;
    }

    static void setDepthTest(bool on);

    inline static bool getDepthTest()
    {
        return s_depthTest;
    }

    static void setBlendingMode(GraphicsApi::BlendingMode);

    inline static GraphicsApi::BlendingMode getBlendingMode()
    {
        return s_currBlendingMode;
    }

   protected:
    inline static bool         s_wireframe        = false;
    inline static bool         s_depthTest        = false;
    inline static BlendingMode s_currBlendingMode = BlendingMode::alphaBlend;
};
}  // namespace nimbus