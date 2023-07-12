#include "mesh.hpp"

namespace nimbus
{

Mesh::Mesh(std::vector<Vertex>   verticies,
           std::vector<uint32_t> indicies,
           std::vector<Texture*> textures,
           bool                  normalize)
    : m_vertices(verticies), m_indices(indicies), m_normalize(normalize)
{
    if (textures.size() > Texture::s_getMaxTextures())
    {
        NM_CORE_ASSERT(
            0, "Too many textures. Did you call Texture::s_setMaxTextures?");
    }

    m_textures = textures;

    _setupMesh();
}

Mesh::Mesh(std::vector<Vertex>   verticies,
           std::vector<Texture*> textures,
           bool                  normalize)
    : m_vertices(verticies), m_normalize(normalize)
{
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
    _setupMesh();
}

Mesh::Mesh(std::vector<Vertex> verticies, bool normalize)
    : m_vertices(verticies), m_normalize(normalize)
{
    _setupMesh();
}

void Mesh::setShader(Shader* p_shader)
{
    mp_shader = p_shader;
}

const Shader* Mesh::getShader() const
{
    return mp_shader;
}

void Mesh::draw() const
{
    uint32_t  diffIndex   = 0;
    uint32_t  specIndex   = 0;
    uint32_t  ambIndex    = 0;
    uint32_t  normalIndex = 0;
    uint32_t  heightIndex = 0;
    uint32_t* typeIndex   = nullptr;

    // go through and bind all of our textures
    for (uint32_t i = 0; i < m_textures.size(); i++)
    {
        Texture* p_texture = m_textures[i];

        switch (p_texture->m_type)
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

        const std::string& uniformNm = p_texture->getUniformNm(*typeIndex++);

        mp_shader->setInt(uniformNm.c_str(), i);

        p_texture->bind(i);
    }

    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    glBindVertexArray(m_vao);

    if (m_hasEbo)
    {
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
    }

    // unbind
    glBindVertexArray(0);
    Texture::s_unbind();
}

void Mesh::_setupMesh()
{
    // TODO, abstract buffers themselves to a class
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glBufferData(GL_ARRAY_BUFFER,
                 m_vertices.size() * sizeof(Vertex),
                 &m_vertices[0],
                 GL_STATIC_DRAW);

    if (m_indices.size() > 0)
    {
        glGenBuffers(1, &m_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     m_indices.size() * sizeof(uint32_t),
                     &m_indices[0],
                     GL_STATIC_DRAW);
        m_hasEbo = true;
    }

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          m_normalize ? GL_TRUE : GL_FALSE,
                          sizeof(Vertex),
                          (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          m_normalize ? GL_TRUE : GL_FALSE,
                          sizeof(Vertex),
                          (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,
                          2,
                          GL_FLOAT,
                          m_normalize ? GL_TRUE : GL_FALSE,
                          sizeof(Vertex),
                          (void*)offsetof(Vertex, texCoords));

    // unbind
    glBindVertexArray(0);
}

};  // namespace nimbus
