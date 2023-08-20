#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/renderer/graphicsApi.hpp"

#include "nimbus/renderer/texture.hpp"

#include "nimbus/platform/gl/glGraphicsApi.hpp"

namespace nimbus
{

void GraphicsApi::init()
{
    static std::once_flag initFlag;
    std::call_once(initFlag, []() { GlGraphicsApi::init(); });
}

void GraphicsApi::drawElements(ref<VertexArray> p_vertexArray, u32_t vertexCount)
{
    GlGraphicsApi::drawElements(p_vertexArray, vertexCount);
}

void GraphicsApi::drawArrays(ref<VertexArray> p_vertexArray, u32_t vertexCount)
{
    GlGraphicsApi::drawArrays(p_vertexArray, vertexCount);
}

void GraphicsApi::drawElementsInstanced(ref<VertexArray> p_vertexArray, u32_t instanceCount, u32_t vertexCount)
{
    GlGraphicsApi::drawElementsInstanced(p_vertexArray, instanceCount, vertexCount);
}

void GraphicsApi::drawArraysInstanced(ref<VertexArray> p_vertexArray, u32_t instanceCount, u32_t vertexCount)
{
    GlGraphicsApi::drawArraysInstanced(p_vertexArray, instanceCount, vertexCount);
}

void GraphicsApi::clear()
{
    GlGraphicsApi::clear();
}

void GraphicsApi::clearColor(glm::vec4 color)
{
    GlGraphicsApi::clearColor(color);
}

void GraphicsApi::setViewportSize(int x, int y, int w, int h)
{
    GlGraphicsApi::setViewportSize(x, y, w, h);
}

void GraphicsApi::setWireframe(bool on)
{
    GlGraphicsApi::setWireframe(on);
}

void GraphicsApi::setDepthTest(bool on)
{
    GlGraphicsApi::setDepthTest(on);
}

void GraphicsApi::setBlendingMode(GraphicsApi::BlendingMode mode)
{
    GlGraphicsApi::setBlendingMode(mode);
}

}  // namespace nimbus