#pragma once

#include "buffer.hpp"

namespace nimbus
{
class RendererApi
{
   public:
    enum class BlendingMode
    {
        ADDITIVE,               // GL_ONE, GL_ONE
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

    static bool getWireframe();

    static void setDepthTest(bool on);

    static bool getDepthTest();

    static void setBlendingMode(RendererApi::BlendingMode);

    static RendererApi::BlendingMode getBlendingMode();

   private:
    inline static bool         s_wireframeOn      = false;
    inline static bool         s_depthTest        = false;
    inline static BlendingMode s_currBlendingMode = BlendingMode::ALPHA_BLEND;

    static void _enableGlErrPrint();
};
}  // namespace nimbus