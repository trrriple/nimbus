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
    m_vpMatrix = camera.getViewProjection();
}

void Renderer::submit(const ref<Shader>&      p_shader,
                      const ref<VertexArray>& p_vertexArray,
                      const glm::mat4&        p_model,
                      int32_t                 count)
{
    NM_PROFILE();
    
    // TODO fix count for drawArrays

    p_shader->use();

    p_shader->setMat4("u_model", p_model);
    p_shader->setMat4("u_viewProjection", m_vpMatrix);
    
    if (p_vertexArray->getIndexBuffer())
    {
        if(count == k_detectCountIfPossible)
        {
            RendererApi::drawElements(p_vertexArray);
        }
        else
        {
             RendererApi::drawElements(p_vertexArray, count);
        }
    }
    else
    {
        NM_CORE_ASSERT_STATIC(
            (count != k_detectCountIfPossible),
            "Render submissions without IBO requires a vertex count!");
        
        RendererApi::drawArrays(p_vertexArray, count);
    }
}

}  // namespace nimbus