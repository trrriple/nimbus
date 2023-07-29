#include "nimbus/nmpch.hpp"
#include "nimbus/core.hpp"

#include "nimbus/renderer/graphicsApi.hpp"

#include "nimbus/renderer/texture.hpp"

#include "platform/gl/glGraphicsApi.hpp"

namespace nimbus
{

void GraphicsApi::init()
{
    static std::once_flag initFlag;
    std::call_once(initFlag, []() { GlGraphicsApi::init(); });
}

void GraphicsApi::drawElements(const ref<VertexArray>& p_vertexArray,
                               uint32_t                vertexCount)
{
    GlGraphicsApi::drawElements(p_vertexArray, vertexCount);
}

void GraphicsApi::drawArrays(const ref<VertexArray>& p_vertexArray,
                             uint32_t                vertexCount)
{
    GlGraphicsApi::drawArrays(p_vertexArray, vertexCount);
}

void GraphicsApi::drawElementsInstanced(const ref<VertexArray>& p_vertexArray,
                                        uint32_t                instanceCount,
                                        uint32_t                vertexCount)
{
    GlGraphicsApi::drawElementsInstanced(
        p_vertexArray, instanceCount, vertexCount);
}

void GraphicsApi::drawArraysInstanced(const ref<VertexArray>& p_vertexArray,
                                      uint32_t                instanceCount,
                                      uint32_t                vertexCount)
{
    GlGraphicsApi::drawArraysInstanced(
        p_vertexArray, instanceCount, vertexCount);
}

void GraphicsApi::clear()
{
    GlGraphicsApi::clear();
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