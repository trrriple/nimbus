#include "platform/rendererApi.hpp"

#include "core.hpp"
#include "nmpch.hpp"
#include "renderer/texture.hpp"

namespace nimbus
{

void RendererApi::init()
{
    static std::once_flag initFlag;
    std::call_once(
        initFlag,
        []()
        {
            NM_PROFILE_DETAIL();

            Log::coreInfo("Vendor:   %s", glGetString(GL_VENDOR));
            Log::coreInfo("Renderer: %s", glGetString(GL_RENDERER));
            Log::coreInfo("Version:  %s", glGetString(GL_VERSION));

            if (s_depthTest)
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
            Log::coreInfo("Max number of vertex attributes supported: %i",
                          numAttributes);

            int maxTextureUnits;
            glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

            Texture::s_setMaxTextures(maxTextureUnits);

            Log::coreInfo("Max number of Texture Units: supported: %i",
                          Texture::s_getMaxTextures());
        });
}

void RendererApi::drawElements(const ref<VertexArray>& p_vertexArray,
                               uint32_t                vertexCount)
{
    NM_PROFILE_DETAIL();
    uint32_t count = vertexCount ? vertexCount
                                 : p_vertexArray->getIndexBuffer()->getCount();

    p_vertexArray->bind();
    glDrawElements(GL_TRIANGLES,
                   count,
                   p_vertexArray->getIndexBuffer()->getType(),
                   nullptr);
}

void RendererApi::drawArrays(const ref<VertexArray>& p_vertexArray,
                             uint32_t                vertexCount)
{
    NM_PROFILE_DETAIL();
    uint32_t count
        = vertexCount ? vertexCount : p_vertexArray->getExpectedVertexCount();

    p_vertexArray->bind();
    glDrawArrays(GL_TRIANGLES, 0, count);
}

void RendererApi::drawElementsInstanced(const ref<VertexArray>& p_vertexArray,
                                        uint32_t                instanceCount,
                                        uint32_t                vertexCount)
{
    NM_PROFILE_DETAIL();
    uint32_t count = vertexCount ? vertexCount
                                 : p_vertexArray->getIndexBuffer()->getCount();

    p_vertexArray->bind();
    glDrawElementsInstanced(GL_TRIANGLES,
                            count,
                            p_vertexArray->getIndexBuffer()->getType(),
                            nullptr,
                            instanceCount);
}

void RendererApi::drawArraysInstanced(const ref<VertexArray>& p_vertexArray,
                                      uint32_t                instanceCount,
                                      uint32_t                vertexCount)
{
    NM_PROFILE_DETAIL();
    uint32_t count
        = vertexCount ? vertexCount : p_vertexArray->getExpectedVertexCount();

    p_vertexArray->bind();
    glDrawArraysInstanced(GL_TRIANGLES, 0, count, instanceCount);
}

void RendererApi::clear()
{
    NM_PROFILE_TRACE();

    glClear(GL_COLOR_BUFFER_BIT | (GL_DEPTH_BUFFER_BIT && getDepthTest()));
}

void RendererApi::setViewportSize(int x, int y, int w, int h)
{
    NM_PROFILE_TRACE();

    glViewport(x, y, w, h);
}

void RendererApi::setWireframe(bool on)
{
    NM_PROFILE_TRACE();

    if (on != s_wireframeOn)
    {
        if (!on)
        {
            Log::coreInfo("Wireframe off");
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else
        {
            Log::coreInfo("Wireframe on");
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        s_wireframeOn = on;
    }
}

bool RendererApi::getWireframe()
{
    NM_PROFILE_TRACE();

    return s_wireframeOn;
}

void RendererApi::setDepthTest(bool on)
{
    NM_PROFILE_TRACE();

    if (!on)
    {
        glDisable(GL_DEPTH_TEST);
    }
    else
    {
        glEnable(GL_DEPTH_TEST);
    }
    s_depthTest = on;
}

bool RendererApi::getDepthTest()
{
    NM_PROFILE_TRACE();

    return s_depthTest;
}

void RendererApi::setBlendingMode(RendererApi::BlendingMode mode)
{
    if (mode == s_currBlendingMode)
    {
        // no reason to switch to same value
        return;
    }

    switch (mode)
    {
        case BlendingMode::ADDITIVE:
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        case BlendingMode::SUBTRACT:
            glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
            break;
        case BlendingMode::MULTIPLY:
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
            break;
        case BlendingMode::SCREEN:
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
            break;
        case BlendingMode::REPLACE:
            glBlendFunc(GL_ONE, GL_ZERO);
            break;
        case BlendingMode::ALPHA_BLEND:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case BlendingMode::ALPHA_PREMULTIPLIED:
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case BlendingMode::SOURCE_ALPHA_ADDITIVE:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;
        default:
            NM_CORE_ASSERT_STATIC(0, "Invalid blending mode %i", mode);
    }

    s_currBlendingMode = mode;
}

RendererApi::BlendingMode RendererApi::getBlendingMode()
{
    return s_currBlendingMode;
}

void APIENTRY _glDebugOutput(GLenum       source,
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

    Log::coreError("Debug message (%d):  %s", id, message);
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            Log::coreError("Source: API");
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            Log::coreError("Source: Window System");
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            Log::coreError("Source: Shader Compiler");
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            Log::coreError("Source: Third Party");
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            Log::coreError("Source: Application");
            break;
        case GL_DEBUG_SOURCE_OTHER:
            Log::coreError("Source: Other");
            break;
    }

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            Log::coreError("Type: Error");
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            Log::coreError("Type: Deprecated Behaviour");
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            Log::coreError("Type: Undefined Behaviour");
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            Log::coreError("Type: Portability");
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            Log::coreError("Type: Performance");
            break;
        case GL_DEBUG_TYPE_MARKER:
            Log::coreError("Type: Marker");
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            Log::coreError("Type: Push Group");
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            Log::coreError("Type: Pop Group");
            break;
        case GL_DEBUG_TYPE_OTHER:
            Log::coreError("Type: Other");
            break;
    }

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            Log::coreError("Severity: high");
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            Log::coreError("Severity: medium");
            break;
        case GL_DEBUG_SEVERITY_LOW:
            Log::coreError("Severity: low");
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            Log::coreError("Severity: notification");
            break;
    }
}

void RendererApi::_enableGlErrPrint()
{
    Log::coreInfo("GL Debug Enabled");
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(_glDebugOutput, nullptr);
    glDebugMessageControl(
        GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
}

}  // namespace nimbus