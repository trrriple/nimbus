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
            NM_CORE_INFO("Wireframe off\n");
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else
        {
            NM_CORE_INFO("Wireframe on\n");
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
        NM_CORE_INFO("Depth test off\n");
        glDisable(GL_DEPTH_TEST);
    }
    else
    {
        NM_CORE_INFO("Depth test on\n");
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

    NM_CORE_ASSERT(
        m_context, "Failed to created OpenGL Context %s\n", SDL_GetError());

    // setup GLAD
    if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
        NM_CORE_ASSERT(0, "Failed to initialize Glad %s\n");
    }
    
    NM_CORE_INFO("Vendor:   %s\n", glGetString(GL_VENDOR));
    NM_CORE_INFO("Renderer: %s\n", glGetString(GL_RENDERER));
    NM_CORE_INFO("Version:  %s\n", glGetString(GL_VERSION));

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
    NM_CORE_INFO("Max number of vertex attributes supported: %i\n", numAttributes);

    int maxTextureUnits;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

    Texture::s_setMaxTextures(maxTextureUnits);

    NM_CORE_INFO("Max number of Texture Units: supported: %i\n",
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

    NM_CORE_ERROR("Debug message (%d):  %s\n", id, message);
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            NM_CORE_ERROR("Source: API\n");
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            NM_CORE_ERROR("Source: Window System\n");
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            NM_CORE_ERROR("Source: Shader Compiler\n");
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            NM_CORE_ERROR("Source: Third Party\n");
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            NM_CORE_ERROR("Source: Application\n");
            break;
        case GL_DEBUG_SOURCE_OTHER:
            NM_CORE_ERROR("Source: Other\n");
            break;
    }

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            NM_CORE_ERROR("Type: Error\n");
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            NM_CORE_ERROR("Type: Deprecated Behaviour\n");
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            NM_CORE_ERROR("Type: Undefined Behaviour\n");
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            NM_CORE_ERROR("Type: Portability\n");
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            NM_CORE_ERROR("Type: Performance\n");
            break;
        case GL_DEBUG_TYPE_MARKER:
            NM_CORE_ERROR("Type: Marker\n");
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            NM_CORE_ERROR("Type: Push Group\n");
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            NM_CORE_ERROR("Type: Pop Group\n");
            break;
        case GL_DEBUG_TYPE_OTHER:
            NM_CORE_ERROR("Type: Other\n");
            break;
    }

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            NM_CORE_ERROR("Severity: high\n");
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            NM_CORE_ERROR("Severity: medium\n");
            break;
        case GL_DEBUG_SEVERITY_LOW:
            NM_CORE_ERROR("Severity: low\n");
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            NM_CORE_ERROR("Severity: notification\n");
            break;
    }
}

void Renderer::_enableGlErrPrint()
{
    NM_CORE_INFO("GL Debug Enabled\n");
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(_glDebugOutput, nullptr);
    glDebugMessageControl(
        GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
}

}  // namespace nimbus