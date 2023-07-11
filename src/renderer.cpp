#include "renderer.hpp"

namespace nimbus
{

Renderer::Renderer()
{
}

Renderer::Renderer(SDL_Window* p_window) : mp_window(p_window)
{
}

void Renderer::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::render()
{
    SDL_GL_SwapWindow(mp_window);

    _calcFramerate();
}

void Renderer::setVSync(bool on)
{
    if (on != m_VSyncOn)
    {
        SDL_GL_SetSwapInterval(on);
        m_VSyncOn = on;
    }
}

bool Renderer::getVSync()
{
    return m_VSyncOn;
}

void Renderer::setWireframe(bool on)
{
    if (on != m_wireframeOn)
    {
        if (!on)
        {
            NM_LOG("Wireframe off\n");
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else
        {
            NM_LOG("Wireframe on\n");
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        m_wireframeOn = on;
    }
}

bool Renderer::getWireframe()
{
    return m_wireframeOn;
}

void Renderer::setDepthTest(bool on)
{
    if (!on)
    {
        NM_LOG("Depth test off\n");
        glDisable(GL_DEPTH_TEST);
    }
    else
    {
        NM_LOG("Depth test on\n");
        glEnable(GL_DEPTH_TEST);
    }
    m_depthTest = on;
}

bool Renderer::getDepthTest()
{
    return m_depthTest;
}

void Renderer::init()
{
    m_context = SDL_GL_CreateContext(mp_window);

    if (m_context == nullptr)
    {
        NM_ELOG(0, "Failed to created OpenGL Context %s\n", SDL_GetError());
        throw std::runtime_error("Failed initialize SDL\n");
    }
    // setup GLAD
    if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
        throw std::runtime_error("Failed initialize GLAD\n");
    }
    NM_LOG("Vendor:   %s\n", glGetString(GL_VENDOR));
    NM_LOG("Renderer: %s\n", glGetString(GL_RENDERER));
    NM_LOG("Version:  %s\n", glGetString(GL_VERSION));

    // enable v-sync
    SDL_GL_SetSwapInterval(m_VSyncOn);

    if (m_depthTest)
    {
        glEnable(GL_DEPTH_TEST);
    }

    // glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // tell GL the size of the window
    int w, h;
    SDL_GetWindowSize(mp_window, &w, &h);
    glViewport(0, 0, w, h);

    // set the gl clear color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        _enableGlErrPrint();
    }

    int numAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttributes);
    NM_LOG("Max number of vertex attributes supported: %i\n", numAttributes);

    int maxTextureUnits;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

    Texture::s_setMaxTextures(maxTextureUnits);

    NM_LOG("Max number of Texture Units: supported: %i\n",
           Texture::s_getMaxTextures());
}

void Renderer::_calcFramerate()
{
    float tNow_s = core::getTime_s();

    static uint32_t frameCount    = 0;
    static float    tLastFrame_s  = 0;
    static float    samplesPeriod = 0.0;

    m_tFrame_s   = tNow_s - tLastFrame_s;
    tLastFrame_s = tNow_s;

    frameCount++;

    samplesPeriod += m_tFrame_s;

    if (samplesPeriod >= 0.5)
    {
        m_fps         = (float)frameCount / (samplesPeriod);
        frameCount    = 0;
        samplesPeriod = 0;
    }
}

void APIENTRY Renderer::_glDebugOutput(GLenum       source,
                                       GLenum       type,
                                       unsigned int id,
                                       GLenum       severity,
                                       GLsizei      length,
                                       const char*  message,
                                       const void*  userParam)
{
    UNUSED(length);
    UNUSED(userParam);

    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    NM_ELOG(0, "Debug message (%d):  %s\n", id, message);
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            NM_ELOG(0, "Source: API\n");
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            NM_ELOG(0, "Source: Window System\n");
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            NM_ELOG(0, "Source: Shader Compiler\n");
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            NM_ELOG(0, "Source: Third Party\n");
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            NM_ELOG(0, "Source: Application\n");
            break;
        case GL_DEBUG_SOURCE_OTHER:
            NM_ELOG(0, "Source: Other\n");
            break;
    }

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            NM_ELOG(0, "Type: Error\n");
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            NM_ELOG(0, "Type: Deprecated Behaviour\n");
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            NM_ELOG(0, "Type: Undefined Behaviour\n");
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            NM_ELOG(0, "Type: Portability\n");
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            NM_ELOG(0, "Type: Performance\n");
            break;
        case GL_DEBUG_TYPE_MARKER:
            NM_ELOG(0, "Type: Marker\n");
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            NM_ELOG(0, "Type: Push Group\n");
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            NM_ELOG(0, "Type: Pop Group\n");
            break;
        case GL_DEBUG_TYPE_OTHER:
            NM_ELOG(0, "Type: Other\n");
            break;
    }

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            NM_ELOG(0, "Severity: high\n");
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            NM_ELOG(0, "Severity: medium\n");
            break;
        case GL_DEBUG_SEVERITY_LOW:
            NM_ELOG(0, "Severity: low\n");
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            NM_ELOG(0, "Severity: notification\n");
            break;
    }
}

void Renderer::_enableGlErrPrint()
{
    NM_LOG("GL Debug Enabled\n");
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(_glDebugOutput, nullptr);
    glDebugMessageControl(
        GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
}

}  // namespace nimbus