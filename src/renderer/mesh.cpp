#include "nmpch.hpp"
#include "core.hpp"

#include "renderer/mesh.hpp"
#include "renderer/renderer.hpp"

namespace nimbus
{

Mesh::Mesh(std::vector<Vertex>        verticies,
           std::vector<uint32_t>      indicies,
           std::vector<ref<Texture>>  textures,
           bool                       normalize)
    : m_vertices(verticies), m_indices(indicies), m_normalize(normalize)
{
    NM_PROFILE_DETAIL();

    if (textures.size() > Texture::s_getMaxTextures())
    {
        NM_CORE_ASSERT(
            0, "Too many textures. Did you call Texture::s_setMaxTextures?");
    }

    m_textures = textures;

    _setupMesh();
}

Mesh::Mesh(std::vector<Vertex>        verticies,
           std::vector<ref<Texture>>  textures,
           bool                       normalize)
    : m_vertices(verticies), m_normalize(normalize)
{
    NM_PROFILE_DETAIL();

    if (textures.size() > Texture::s_getMaxTextures())
    {
        NM_CORE_ASSERT(
            0, "Too many textures. Did you call Texture::s_setMaxTextures?");
    }

    m_textures = textures;

    _setupMesh();
}

Mesh::Mesh(std::vector<Vertex>   verticies,
           std::vector<uint32_t> indicies,
           bool                  normalize)
    : m_vertices(verticies), m_indices(indicies), m_normalize(normalize)
{
    NM_PROFILE_DETAIL();

    _setupMesh();
}

Mesh::Mesh(std::vector<Vertex> verticies, bool normalize)
    : m_vertices(verticies), m_normalize(normalize)
{
    NM_PROFILE_DETAIL();

    _setupMesh();
}

void Mesh::setShader(ref<Shader>& p_shader)
{
    NM_PROFILE_TRACE();

    mp_shader = p_shader;
}

ref<Shader>& Mesh::getShader()
{
    NM_PROFILE_TRACE();

    return mp_shader;
}

void Mesh::draw(glm::mat4& model) const
{
    NM_PROFILE();


    uint32_t  diffIndex   = 0;
    uint32_t  specIndex   = 0;
    uint32_t  ambIndex    = 0;
    uint32_t  normalIndex = 0;
    uint32_t  heightIndex = 0;
    uint32_t* typeIndex   = nullptr;

    // go through and bind all of our textures
    for (uint32_t i = 0; i < m_textures.size(); i++)
    {
        switch (m_textures[i]->m_type)
        {
            case (Texture::Type::DIFFUSE):
            {
                typeIndex = &diffIndex;
                break;
            }
            case (Texture::Type::SPECULAR):
            {
                typeIndex = &specIndex;
                break;
            }
            case (Texture::Type::AMBIENT):
            {
                typeIndex = &ambIndex;
                break;
            }
            case (Texture::Type::NORMAL):
            {
                typeIndex = &normalIndex;
                break;
            }
            case (Texture::Type::HEIGHT):
            {
                typeIndex = &heightIndex;
                break;
            }
            default:
            {
                NM_CORE_ASSERT(0, "Texture type unsupported.");
            }
        }

        const std::string& uniformNm
            = m_textures[i]->getUniformNm(*typeIndex++);

        mp_shader->setInt(uniformNm.c_str(), i);

        m_textures[i]->bind(i);
    }

    // TODO do I need this?
    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    mp_vao->bind();

    // if (m_hasEbo)
    // {
    //     glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    // }
    // else
    // {
    //     glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
    // }

    Renderer::submit(mp_shader, mp_vao, model, m_vertices.size());

    // unbind
    glBindVertexArray(0);
    Texture::s_unbind();
}

void Mesh::_setupMesh()
{
    mp_vbo = makeRef<VertexBuffer>(&m_vertices[0],
                                   m_vertices.size() * sizeof(Vertex));

    if (!m_normalize)
    {
        mp_vbo->setFormat(k_vboFormat);
    }
    else
    {
        mp_vbo->setFormat(k_vboFormatNormalize);
    }

    mp_vao = makeRef<VertexArray>();
    mp_vao->addVertexBuffer(mp_vbo);

    if (m_indices.size() > 0)
    {
        mp_ebo   = makeRef<IndexBuffer>(&m_indices[0], m_indices.size());
        m_hasEbo = true;
    }
}

};  // namespace nimbus
