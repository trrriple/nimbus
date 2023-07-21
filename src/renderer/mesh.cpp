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

    // draw mesh
    mp_vao->bind();

    Renderer::submit(mp_shader, mp_vao, model);
}

void Mesh::_setupMesh()
{
    ref<VertexBuffer> p_vbo = makeRef<VertexBuffer>(
        &m_vertices[0], m_vertices.size() * sizeof(Vertex));

    if (!m_normalize)
    {
        p_vbo->setFormat(k_vboFormat);
    }
    else
    {
        p_vbo->setFormat(k_vboFormatNormalize);
    }

    mp_vao = makeRef<VertexArray>();
    mp_vao->addVertexBuffer(p_vbo);
    if (m_indices.size() > 0)

    {
        // determine type to use for indicies
        uint32_t maxIndex
            = *std::max_element(m_indices.begin(), m_indices.end());

        if (maxIndex < std::numeric_limits<uint8_t>::max())
        {
            std::vector<uint8_t> indices8;
            indices8.reserve(m_indices.size());

            for (uint32_t index : m_indices)
            {
                indices8.push_back(static_cast<uint8_t>(index));
            }

            mp_vao->setIndexBuffer(
                makeRef<IndexBuffer>(&indices8[0], indices8.size()));
        }
        else if (maxIndex < std::numeric_limits<uint16_t>::max())
        {
            std::vector<uint16_t> indices16;
            indices16.reserve(m_indices.size());

            for (uint32_t index : m_indices)
            {
                indices16.push_back(static_cast<uint16_t>(index));
            }

            mp_vao->setIndexBuffer(
                makeRef<IndexBuffer>(&indices16[0], indices16.size()));
        }
        else  // we pass in uint32_t so it can't be bigger then that
        {
            std::vector<uint8_t> indices32;
            indices32.reserve(m_indices.size());

            for (uint32_t index : m_indices)
            {
                indices32.push_back(static_cast<uint32_t>(index));
            }

            mp_vao->setIndexBuffer(
                makeRef<IndexBuffer>(&indices32[0], indices32.size()));
        }
    }
}

};  // namespace nimbus
