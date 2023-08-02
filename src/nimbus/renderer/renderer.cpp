#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/renderer/renderer.hpp"
#include "nimbus/renderer/graphicsApi.hpp"

#include <sstream>


namespace nimbus
{

void Renderer::s_init()
{
    GraphicsApi::init();
}
void Renderer::s_destroy()
{
}

void Renderer::s_setScene(Camera& camera)
{
    mp_vpMatrix = &camera.getViewProjection();
}

void Renderer::s_submit(const ref<Shader>&      p_shader,
                        const ref<VertexArray>& p_vertexArray,
                        const glm::mat4&        p_model,
                        int32_t                 vertexCount,
                        bool                    setViewProjection)
{
    NM_PROFILE();

    p_shader->bind();
    p_shader->setMat4("u_model", p_model);

    if (setViewProjection)
    {
        if (mp_vpMatrix != nullptr)
        {
            p_shader->setMat4("u_viewProjection", *mp_vpMatrix);
        }
        else
        {
            Log::coreError(
                "Renderer::s_setScene() must be called before submitting to "
                "render!");
        }
    }

    // do we have an index buffer?
    if (p_vertexArray->getIndexBuffer())
    {
        // we do, so drawElements
        if (vertexCount == k_detectCountIfPossible)
        {
            GraphicsApi::drawElements(p_vertexArray);
        }
        else
        {
            GraphicsApi::drawElements(p_vertexArray, vertexCount);
        }
    }
    else
    {
        // we don't so drawArrays
        if (vertexCount == k_detectCountIfPossible)
        {
            GraphicsApi::drawArrays(p_vertexArray);
        }
        else
        {
            GraphicsApi::drawArrays(p_vertexArray, vertexCount);
        }
    }
}

void Renderer::s_submit(const ref<Shader>&      p_shader,
                        const ref<VertexArray>& p_vertexArray,
                        int32_t                 vertexCount,
                        bool                    setViewProjection)
{
    NM_PROFILE();

    p_shader->bind();

    if (setViewProjection)
    {
        if (mp_vpMatrix != nullptr)
        {
            p_shader->setMat4("u_viewProjection", *mp_vpMatrix);
        }
        else
        {
            Log::coreError(
                "Renderer::s_setScene() must be called before submitting to "
                "render!");
        }
    }

    // do we have an index buffer?
    if (p_vertexArray->getIndexBuffer())
    {
        // we do, so drawElements
        if (vertexCount == k_detectCountIfPossible)
        {
            GraphicsApi::drawElements(p_vertexArray);
        }
        else
        {
            GraphicsApi::drawElements(p_vertexArray, vertexCount);
        }
    }
    else
    {
        // we don't so drawArrays
        if (vertexCount == k_detectCountIfPossible)
        {
            GraphicsApi::drawArrays(p_vertexArray);
        }
        else
        {
            GraphicsApi::drawArrays(p_vertexArray, vertexCount);
        }
    }
}

void Renderer::s_submitInstanced(const ref<Shader>&      p_shader,
                                 const ref<VertexArray>& p_vertexArray,
                                 int32_t                 instanceCount,
                                 const glm::mat4&        p_model,
                                 int32_t                 vertexCount,
                                 bool                    setViewProjection)
{
    NM_PROFILE();

    p_shader->bind();
    p_shader->setMat4("u_model", p_model);

    if (setViewProjection)
    {
        if (mp_vpMatrix != nullptr)
        {
            p_shader->setMat4("u_viewProjection", *mp_vpMatrix);
        }
        else
        {
            Log::coreError(
                "Renderer::s_setScene() must be called before submitting to "
                "render!");
        }
    }

    // do we have an index buffer?
    if (p_vertexArray->getIndexBuffer())
    {
        // we do, so drawElements
        if (vertexCount == k_detectCountIfPossible)
        {
            GraphicsApi::drawElementsInstanced(p_vertexArray, instanceCount);
        }
        else
        {
            GraphicsApi::drawElementsInstanced(
                p_vertexArray, instanceCount, vertexCount);
        }
    }
    else
    {
        // we don't so drawArrays
        if (vertexCount == k_detectCountIfPossible)
        {
            GraphicsApi::drawArraysInstanced(p_vertexArray, instanceCount);
        }
        else
        {
            GraphicsApi::drawArraysInstanced(
                p_vertexArray, instanceCount, vertexCount);
        }
    }
}

void Renderer::s_submitInstanced(const ref<Shader>&      p_shader,
                                 const ref<VertexArray>& p_vertexArray,
                                 int32_t                 instanceCount,
                                 int32_t                 vertexCount,
                                 bool                    setViewProjection)
{
    NM_PROFILE();

    p_shader->bind();

    if (setViewProjection)
    {
        if (mp_vpMatrix != nullptr)
        {
            p_shader->setMat4("u_viewProjection", *mp_vpMatrix);
        }
        else
        {
            Log::coreError(
                "Renderer::s_setScene() must be called before submitting to "
                "render!");
        }
    }

    // do we have an index buffer?
    if (p_vertexArray->getIndexBuffer())
    {
        // we do, so drawElements
        if (vertexCount == k_detectCountIfPossible)
        {
            GraphicsApi::drawElementsInstanced(p_vertexArray, instanceCount);
        }
        else
        {
            GraphicsApi::drawElementsInstanced(
                p_vertexArray, instanceCount, vertexCount);
        }
    }
    else
    {
        // we don't so drawArrays
        if (vertexCount == k_detectCountIfPossible)
        {
            GraphicsApi::drawArraysInstanced(p_vertexArray, instanceCount);
        }
        else
        {
            GraphicsApi::drawArraysInstanced(
                p_vertexArray, instanceCount, vertexCount);
        }
    }
}

}  // namespace nimbus