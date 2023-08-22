#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/platform/gl/glGraphicsApi.hpp"
#include "nimbus/renderer/renderer.hpp"

#include "nimbus/renderer/texture.hpp"

#include "glad.h"

namespace nimbus
{

void GlGraphicsApi::init()
{
    NB_PROFILE_DETAIL();

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
        NB_CORE_ASSERT_STATIC(0, "Failed to initialize Glad %s");
    }

    Log::coreInfo("Vendor:   %s", glGetString(GL_VENDOR));
    Log::coreInfo("Renderer: %s", glGetString(GL_RENDERER));
    Log::coreInfo("Version:  %s", glGetString(GL_VERSION));

    if (s_depthTest)
    {
        glEnable(GL_DEPTH_TEST);
    }

    glEnable(GL_LINE_SMOOTH);

    glDisable(GL_DITHER);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // set the gl clear color
    glClearColor(0.15f, 0.16f, 0.13f, 1.0f);

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        _enableGlErrPrint();
    }

    int numAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttributes);
    Log::coreInfo("Max number of vertex attributes supported: %i", numAttributes);

    int maxTextureUnits;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

    Texture::s_setMaxTextures(maxTextureUnits);

    Log::coreInfo("Max number of Texture Units: supported: %i", Texture::s_getMaxTextures());
}

void GlGraphicsApi::drawElements(ref<VertexArray> p_vertexArray, u32_t vertexCount)
{
    NB_PROFILE_DETAIL();
    u32_t count = vertexCount ? vertexCount : p_vertexArray->getIndexBuffer()->getCount();

    p_vertexArray->bind();
    u32_t type = p_vertexArray->getIndexBuffer()->getType();

    Renderer::s_submit([count, type]() { glDrawElements(GL_TRIANGLES, count, type, nullptr); });
}

void GlGraphicsApi::drawArrays(ref<VertexArray> p_vertexArray, u32_t vertexCount)
{
    NB_PROFILE_DETAIL();
    u32_t count = vertexCount ? vertexCount : p_vertexArray->getExpectedVertexCount();

    p_vertexArray->bind();
    Renderer::s_submit([count]() { glDrawArrays(GL_TRIANGLES, 0, count); });
}

void GlGraphicsApi::drawElementsInstanced(ref<VertexArray> p_vertexArray, u32_t instanceCount, u32_t vertexCount)
{
    NB_PROFILE_DETAIL();
    u32_t count = vertexCount ? vertexCount : p_vertexArray->getIndexBuffer()->getCount();

    p_vertexArray->bind();
    u32_t type = p_vertexArray->getIndexBuffer()->getType();

    Renderer::s_submit([count, instanceCount, type]()
                       { glDrawElementsInstanced(GL_TRIANGLES, count, type, nullptr, instanceCount); });
}

void GlGraphicsApi::drawArraysInstanced(ref<VertexArray> p_vertexArray, u32_t instanceCount, u32_t vertexCount)
{
    NB_PROFILE_DETAIL();
    u32_t count = vertexCount ? vertexCount : p_vertexArray->getExpectedVertexCount();

    p_vertexArray->bind();

    Renderer::s_submit([count, instanceCount]() { glDrawArraysInstanced(GL_TRIANGLES, 0, count, instanceCount); });
}

void GlGraphicsApi::clear()
{
    NB_PROFILE_TRACE();

    u32_t bits = getDepthTest() ? (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) : GL_COLOR_BUFFER_BIT;

    Renderer::s_submit([bits]() { glClear(bits); });
}

void GlGraphicsApi::clearColor(glm::vec4 color)
{
    Renderer::s_submit([color]() { glClearColor(color.r, color.g, color.b, color.a); });
}

void GlGraphicsApi::setViewportSize(int x, int y, int w, int h)
{
    Renderer::s_submit([x, y, w, h]() { glViewport(x, y, w, h); });
}

void GlGraphicsApi::setWireframe(bool on)
{
    NB_PROFILE_TRACE();

    if (on == s_wireframe)
    {
        return;
    }

    if (!on)
    {
        Renderer::s_submit(
            []()
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                s_wireframe = false;
            });
    }
    else
    {
        Renderer::s_submit(
            []()
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                s_wireframe = true;
            });
    }
}

void GlGraphicsApi::setDepthTest(bool on)
{
    NB_PROFILE_TRACE();

    if (on == s_depthTest)
    {
        return;
    }

    if (!on)
    {
        Renderer::s_submit(
            []()
            {
                glDisable(GL_DEPTH_TEST);
                s_depthTest = false;
            });
    }
    else
    {
        Renderer::s_submit(
            []()
            {
                glEnable(GL_DEPTH_TEST);
                s_depthTest = true;
            });
    }
}

void GlGraphicsApi::setBlendingMode(GraphicsApi::BlendingMode mode)
{
    if (mode == s_currBlendingMode)
    {
        // no reason to switch to same value
        return;
    }

    u32_t sFactor = GL_NONE;
    u32_t dFactor = GL_NONE;
    switch (mode)
    {
        case BlendingMode::additive:
            sFactor = GL_ONE;
            dFactor = GL_ONE;
            break;
        case BlendingMode::subtract:
            sFactor = GL_ZERO;
            dFactor = GL_ONE_MINUS_SRC_COLOR;
            break;
        case BlendingMode::multiply:
            sFactor = GL_DST_COLOR;
            dFactor = GL_ZERO;
            break;
        case BlendingMode::screen:
            sFactor = GL_ONE;
            dFactor = GL_ONE_MINUS_SRC_COLOR;
            break;
        case BlendingMode::replace:
            sFactor = GL_ONE;
            dFactor = GL_ZERO;
            break;
        case BlendingMode::alphaBlend:
            sFactor = GL_SRC_ALPHA;
            dFactor = GL_ONE_MINUS_SRC_ALPHA;
            break;
        case BlendingMode::alphaPremultiplied:
            sFactor = GL_ONE;
            dFactor = GL_ONE_MINUS_SRC_ALPHA;
            break;
        case BlendingMode::sourceAlphaAdditive:
            sFactor = GL_SRC_ALPHA;
            dFactor = GL_ONE;
            break;
        default:
            NB_CORE_ASSERT_STATIC(0, "Invalid blending mode %i", mode);
    }

    s_currBlendingMode = mode;

    Renderer::s_submit([sFactor, dFactor]() { glBlendFunc(sFactor, dFactor); });
}

static void APIENTRY _glDebugOutput(GLenum       source,
                                    GLenum       type,
                                    unsigned int id,
                                    GLenum       severity,
                                    GLsizei      length,
                                    const char*  message,
                                    const void*  userParam)
{
    NB_UNUSED(length);
    NB_UNUSED(userParam);

    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204 || id == 131140
        || id == 131220)  // integer framebuffer warning
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

void GlGraphicsApi::_enableGlErrPrint()
{
    Log::coreInfo("GL Debug Enabled");
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(_glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
}

}  // namespace nimbus