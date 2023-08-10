#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/renderer/graphicsApi.hpp"

#include "nimbus/renderer/texture.hpp"

#include "platform/gl/glGraphicsApi.hpp"

namespace nimbus
{

void GraphicsApi::init() noexcept
{
    static std::once_flag initFlag;
    std::call_once(initFlag, []() { GlGraphicsApi::init(); });
}

void GraphicsApi::drawElements(ref<VertexArray> p_vertexArray,
                               uint32_t         vertexCount) noexcept
{
    GlGraphicsApi::drawElements(p_vertexArray, vertexCount);
}

void GraphicsApi::drawArrays(ref<VertexArray> p_vertexArray,
                             uint32_t         vertexCount) noexcept
{
    GlGraphicsApi::drawArrays(p_vertexArray, vertexCount);
}

void GraphicsApi::drawElementsInstanced(ref<VertexArray> p_vertexArray,
                                        uint32_t         instanceCount,
                                        uint32_t         vertexCount) noexcept
{
    GlGraphicsApi::drawElementsInstanced(
        p_vertexArray, instanceCount, vertexCount);
}

void GraphicsApi::drawArraysInstanced(ref<VertexArray> p_vertexArray,
                                      uint32_t         instanceCount,
                                      uint32_t         vertexCount) noexcept
{
    GlGraphicsApi::drawArraysInstanced(
        p_vertexArray, instanceCount, vertexCount);
}

void GraphicsApi::clear() noexcept
{
    GlGraphicsApi::clear();
}

void GraphicsApi::clearColor(glm::vec4 color) noexcept
{
    GlGraphicsApi::clearColor(color);
}

void GraphicsApi::setViewportSize(int x, int y, int w, int h) noexcept
{
    GlGraphicsApi::setViewportSize(x, y, w, h);
}

void GraphicsApi::setWireframe(bool on) noexcept
{
    GlGraphicsApi::setWireframe(on);
}

void GraphicsApi::setDepthTest(bool on) noexcept
{
    GlGraphicsApi::setDepthTest(on);
}

void GraphicsApi::setBlendingMode(GraphicsApi::BlendingMode mode) noexcept
{
    GlGraphicsApi::setBlendingMode(mode);
}

}  // namespace nimbus