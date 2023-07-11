#pragma once

#include "core.hpp"
#include "texture.hpp"

namespace nimbus
{
class Renderer
{
   public:
    SDL_GLContext m_context  = nullptr;
    float         m_tFrame_s = 0.0;
    float         m_fps      = 0.0;

    Renderer();

    Renderer(SDL_Window* p_window);

    void clear();

    void render();

    void setVSync(bool on);

    bool getVSync();

    void setWireframe(bool on);

    bool getWireframe();

    void setDepthTest(bool on);

    bool getDepthTest();

    void init();

   private:
    SDL_Window* mp_window     = nullptr;
    bool        m_VSyncOn     = true;
    bool        m_wireframeOn = false;
    bool        m_depthTest   = false;

    void _calcFramerate();

    static void APIENTRY _glDebugOutput(GLenum       source,
                                        GLenum       type,
                                        unsigned int id,
                                        GLenum       severity,
                                        GLsizei      length,
                                        const char*  message,
                                        const void*  userParam);

    void _enableGlErrPrint();
};
}  // namespace nimbus