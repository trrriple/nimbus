#pragma once

#include "buffer.hpp"

namespace nimbus
{
class RendererApi
{
   public:
    static void init();
    static void clear();

    static void drawElements(const ref<VertexArray>& p_vertexArray,
                             uint32_t                indexCount = 0);

    static void drawArrays(const ref<VertexArray>& p_vertexArrax,
                           uint32_t                vertexCount);

    static void setViewportSize(int x, int y, int w, int h);

    static void setWireframe(bool on);

    static bool getWireframe();

    static void setDepthTest(bool on);

    static bool getDepthTest();

   private:
    inline static bool m_wireframeOn = false;
    inline static bool m_depthTest   = false;

    static void _enableGlErrPrint();
};
}  // namespace nimbus