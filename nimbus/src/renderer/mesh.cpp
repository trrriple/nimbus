#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/renderer/mesh.hpp"

#include "nimbus/renderer/renderer.hpp"

namespace nimbus
{

Mesh::Mesh(std::vector<Vertex>       verticies,
           std::vector<u32_t>        indicies,
           std::vector<ref<Texture>> textures,
           bool                      normalize)
    : m_vertices(verticies), m_indices(indicies), m_normalize(normalize)
{
    NB_PROFILE_DETAIL();

    if (textures.size() > Texture::s_getMaxTextures())
    {
        NB_CORE_ASSERT(0, "Too many textures. Did you call Texture::s_setMaxTextures?");
    }

    m_textures = textures;

    _setupMesh();
    _setupTextureUniforms();
}

Mesh::Mesh(std::vector<Vertex> verticies, std::vector<ref<Texture>> textures, bool normalize)
    : m_vertices(verticies), m_normalize(normalize)
{
    NB_PROFILE_DETAIL();

    if (textures.size() > Texture::s_getMaxTextures())
    {
        NB_CORE_ASSERT(0, "Too many textures. Did you call Texture::s_setMaxTextures?");
    }

    m_textures = textures;

    _setupMesh();
    _setupTextureUniforms();
}

Mesh::Mesh(std::vector<Vertex> verticies, std::vector<u32_t> indicies, bool normalize)
    : m_vertices(verticies), m_indices(indicies), m_normalize(normalize)
{
    NB_PROFILE_DETAIL();

    _setupMesh();
}

Mesh::Mesh(std::vector<Vertex> verticies, bool normalize) : m_vertices(verticies), m_normalize(normalize)
{
    NB_PROFILE_DETAIL();

    _setupMesh();
}

void Mesh::setShader(ref<Shader> p_shader)
{
    NB_PROFILE_TRACE();

    mp_shader = p_shader;
}

ref<Shader> Mesh::getShader()
{
    NB_PROFILE_TRACE();

    return mp_shader;
}

void Mesh::draw(const glm::mat4& model) const
{
    NB_PROFILE();

    u32_t diffIndex   = 0;
    u32_t specIndex   = 0;
    u32_t ambIndex    = 0;
    u32_t normalIndex = 0;
    u32_t heightIndex = 0;

    mp_shader->bind();

    // go through and bind all of our textures
    for (u32_t i = 0; i < m_textures.size(); i++)
    {
        switch (m_textures[i]->getType())
        {
            case (Texture::Type::diffuse):
            {
                mp_shader->setInt(m_texDiffUniformNms[diffIndex++], i);
                break;
            }
            case (Texture::Type::specular):
            {
                mp_shader->setInt(m_texSpecUniformNms[specIndex++], i);
                break;
            }
            case (Texture::Type::ambient):
            {
                mp_shader->setInt(m_texAmbiUniformNms[ambIndex++], i);
                break;
            }
            case (Texture::Type::normal):
            {
                mp_shader->setInt(m_texNormUniformNms[normalIndex++], i);
                break;
            }
            case (Texture::Type::height):
            {
                mp_shader->setInt(m_texHghtUniformNms[heightIndex++], i);
                break;
            }
            default:
            {
                NB_CORE_ASSERT(0, "Texture type unsupported.");
            }
        }

        m_textures[i]->bind(i);
    }

    // draw mesh
    mp_vao->bind();

    // Renderer::s_submit(mp_shader, mp_vao, model);
}

void Mesh::_setupMesh()
{
    ref<VertexBuffer> p_vbo = VertexBuffer::s_create(&m_vertices[0], m_vertices.size() * sizeof(Vertex));

    if (!m_normalize)
    {
        p_vbo->setFormat(k_vboFormat);
    }
    else
    {
        p_vbo->setFormat(k_vboFormatNormalize);
    }

    mp_vao = VertexArray::s_create();
    mp_vao->addVertexBuffer(p_vbo);
    if (m_indices.size() > 0)

    {
        // determine type to use for indicies
        u32_t maxIndex = *std::max_element(m_indices.begin(), m_indices.end());

        if (maxIndex < std::numeric_limits<u8_t>::max())
        {
            std::vector<u8_t> indices8;
            indices8.reserve(m_indices.size());

            for (u32_t index : m_indices)
            {
                indices8.push_back(static_cast<u8_t>(index));
            }

            mp_vao->setIndexBuffer(IndexBuffer::s_create(&indices8[0], indices8.size()));
        }
        else if (maxIndex < std::numeric_limits<u16_t>::max())
        {
            std::vector<u16_t> indices16;
            indices16.reserve(m_indices.size());

            for (u32_t index : m_indices)
            {
                indices16.push_back(static_cast<u16_t>(index));
            }

            mp_vao->setIndexBuffer(IndexBuffer::s_create(&indices16[0], indices16.size()));
        }
        else  // we pass in u32_t so it can't be bigger then that
        {
            std::vector<u8_t> indices32;
            indices32.reserve(m_indices.size());

            for (u32_t index : m_indices)
            {
                indices32.push_back(static_cast<u32_t>(index));
            }

            mp_vao->setIndexBuffer(IndexBuffer::s_create(&indices32[0], indices32.size()));
        }
    }
}

void Mesh::_setupTextureUniforms()
{
    u32_t diffIndex   = 0;
    u32_t specIndex   = 0;
    u32_t ambIndex    = 0;
    u32_t normalIndex = 0;
    u32_t heightIndex = 0;

    for (auto& tex : m_textures)
    {
        switch (tex->getType())
        {
            case (Texture::Type::diffuse):
            {
                std::string name = k_texDiffNm + "_" + std::to_string(diffIndex++);
                m_texDiffUniformNms.push_back(name);
                break;
            }
            case (Texture::Type::specular):
            {
                std::string name = k_texSpecNm + "_" + std::to_string(specIndex++);
                m_texSpecUniformNms.push_back(name);
                break;
            }
            case (Texture::Type::ambient):
            {
                std::string name = k_texAmbiNm + "_" + std::to_string(ambIndex++);
                m_texAmbiUniformNms.push_back(name);
                break;
            }
            case (Texture::Type::normal):
            {
                std::string name = k_texNormNm + "_" + std::to_string(normalIndex++);
                m_texNormUniformNms.push_back(name);
                break;
            }
            case (Texture::Type::height):
            {
                std::string name = k_texHghtNm + "_" + std::to_string(heightIndex++);
                m_texHghtUniformNms.push_back(name);
                break;
            }
        }
    }
}

};  // namespace nimbus
