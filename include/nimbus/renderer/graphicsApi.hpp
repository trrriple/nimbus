#pragma once

#include "nimbus/renderer/buffer.hpp"

namespace nimbus
{
class GraphicsApi
{
   public:
    enum class BlendingMode
    {
        ADDITIVE = 0,           // GL_ONE, GL_ONE
        SUBTRACT,               // GL_ZERO, GL_ONE_MINUS_SRC_COLOR
        MULTIPLY,               // GL_DST_COLOR, GL_ZERO
        SCREEN,                 // GL_ONE, GL_ONE_MINUS_SRC_COLOR
        REPLACE,                // GL_ONE, GL_ZERO
        ALPHA_BLEND,            // GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA
        ALPHA_PREMULTIPLIED,    // GL_ONE, GL_ONE_MINUS_SRC_ALPHA
        SOURCE_ALPHA_ADDITIVE,  // GL_SRC_ALPHA, GL_ONE
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
    inline static BlendingMode s_currBlendingMode = BlendingMode::ALPHA_BLEND;
};
}  // namespace nimbus