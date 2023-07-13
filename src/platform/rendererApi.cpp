#include "platform/rendererApi.hpp"

#include "renderer/texture.hpp"

namespace nimbus
{

void RendererApi::init()
{
    NM_CORE_INFO("Vendor:   %s\n", glGetString(GL_VENDOR));
    NM_CORE_INFO("Renderer: %s\n", glGetString(GL_RENDERER));
    NM_CORE_INFO("Version:  %s\n", glGetString(GL_VERSION));

    if (m_depthTest)
    {
        glEnable(GL_DEPTH_TEST);
    }

    glEnable(GL_LINE_SMOOTH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
    NM_CORE_INFO("Max number of vertex attributes supported: %i\n",
                 numAttributes);

    int maxTextureUnits;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

    Texture::s_setMaxTextures(maxTextureUnits);

    NM_CORE_INFO("Max number of Texture Units: supported: %i\n",
                 Texture::s_getMaxTextures());

}

void RendererApi::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererApi::setViewportSize(int x, int y, int w, int h)
{
    glViewport(x, y, w, h);
}

void RendererApi::setWireframe(bool on)
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

bool RendererApi::getWireframe()
{
    return m_wireframeOn;
}

void RendererApi::setDepthTest(bool on)
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

bool RendererApi::getDepthTest()
{
    return m_depthTest;
}

void APIENTRY RendererApi::_glDebugOutput(GLenum       source,
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

void RendererApi::_enableGlErrPrint()
{
    NM_CORE_INFO("GL Debug Enabled\n");
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(_glDebugOutput, nullptr);
    glDebugMessageControl(
        GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
}

}  // namespace nimbus