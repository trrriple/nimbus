#include "nmpch.hpp"
#include "core.hpp"

#include "renderer/renderer.hpp"
#include "platform/rendererApi.hpp"
#include <sstream>



namespace nimbus
{

void Renderer::init()
{
    RendererApi::init();
}
void Renderer::destroy()
{
}



void Renderer::setScene(Camera& camera)
{
    mp_vpMatrix = &camera.getViewProjection();
}

void Renderer::submit(const ref<Shader>&      p_shader,
                      const ref<VertexArray>& p_vertexArray,
                      const glm::mat4&        p_model,
                      int32_t                 vertexCount)
{
    NM_PROFILE();
    
    p_shader->use();

    p_shader->setMat4("u_model", p_model);
    p_shader->setMat4("u_viewProjection", *mp_vpMatrix);

    
    // do we have an index buffer?
    if (p_vertexArray->getIndexBuffer())
    {
        // we do, so drawElements
        if(vertexCount == k_detectCountIfPossible)
        {
            RendererApi::drawElements(p_vertexArray);
        }
        else
        {
             RendererApi::drawElements(p_vertexArray, vertexCount);
        }
    }
    else
    {
        // we don't so drawArrays
        if (vertexCount == k_detectCountIfPossible)
        {
             RendererApi::drawArrays(p_vertexArray);
        }
        else
        {
             RendererApi::drawArrays(p_vertexArray, vertexCount);
        }
    }
}

void Renderer::submit(const ref<Shader>&      p_shader,
                      const ref<VertexArray>& p_vertexArray,
                      int32_t                 vertexCount)
{
    NM_PROFILE();
    
    p_shader->use();

    p_shader->setMat4("u_viewProjection", *mp_vpMatrix);
    
    // do we have an index buffer?
    if (p_vertexArray->getIndexBuffer())
    {
        // we do, so drawElements
        if(vertexCount == k_detectCountIfPossible)
        {
            RendererApi::drawElements(p_vertexArray);
        }
        else
        {
             RendererApi::drawElements(p_vertexArray, vertexCount);
        }
    }
    else
    {
        // we don't so drawArrays
        if (vertexCount == k_detectCountIfPossible)
        {
             RendererApi::drawArrays(p_vertexArray);
        }
        else
        {
             RendererApi::drawArrays(p_vertexArray, vertexCount);
        }
    }
}


void Renderer::submitInstanced(const ref<Shader>&      p_shader,
                               const ref<VertexArray>& p_vertexArray,
                               int32_t                 instanceCount,
                               const glm::mat4&        p_model,
                               int32_t                 vertexCount)
{
    NM_PROFILE();

    p_shader->use();

    p_shader->setMat4("u_model", p_model);
    p_shader->setMat4("u_viewProjection", *mp_vpMatrix);

    // do we have an index buffer?
    if (p_vertexArray->getIndexBuffer())
    {
        // we do, so drawElements
        if (vertexCount == k_detectCountIfPossible)
        {
             RendererApi::drawElementsInstanced(p_vertexArray, instanceCount);
        }
        else
        {
             RendererApi::drawElementsInstanced(
                 p_vertexArray, instanceCount, vertexCount);
        }
    }
    else
    {
        // we don't so drawArrays
        if (vertexCount == k_detectCountIfPossible)
        {
             RendererApi::drawArraysInstanced(p_vertexArray, instanceCount);
        }
        else
        {
             RendererApi::drawArraysInstanced(
                 p_vertexArray, instanceCount, vertexCount);
        }
    }
}

void Renderer::submitInstanced(const ref<Shader>&      p_shader,
                               const ref<VertexArray>& p_vertexArray,
                               int32_t                 instanceCount,
                               int32_t                 vertexCount)
{
    NM_PROFILE();

    p_shader->use();
    p_shader->setMat4("u_viewProjection", *mp_vpMatrix);

    // do we have an index buffer?
    if (p_vertexArray->getIndexBuffer())
    {
        // we do, so drawElements
        if (vertexCount == k_detectCountIfPossible)
        {
             RendererApi::drawElementsInstanced(p_vertexArray, instanceCount);
        }
        else
        {
             RendererApi::drawElementsInstanced(
                 p_vertexArray, instanceCount, vertexCount);
        }
    }
    else
    {
        // we don't so drawArrays
        if (vertexCount == k_detectCountIfPossible)
        {
             RendererApi::drawArraysInstanced(p_vertexArray, instanceCount);
        }
        else
        {
             RendererApi::drawArraysInstanced(
                 p_vertexArray, instanceCount, vertexCount);
        }
    }
}

}  // namespace nimbus