#pragma once

#include "core.hpp"
#include "platform/buffer.hpp"

namespace nimbus
{
class RendererApi
{
   public:
    static void init();
    static void clear();

    static void setViewportSize(int x, int y, int w, int h);

    static void setWireframe(bool on);

    static bool getWireframe();

    static void setDepthTest(bool on);

    static bool getDepthTest();

   private:
    inline static bool m_wireframeOn = false;
    inline static bool m_depthTest   = false;

    static void APIENTRY _glDebugOutput(GLenum       source,
                                        GLenum       type,
                                        unsigned int id,
                                        GLenum       severity,
                                        GLsizei      length,
                                        const char*  message,
                                        const void*  userParam);

    static void _enableGlErrPrint();
};
}  // namespace nimbus