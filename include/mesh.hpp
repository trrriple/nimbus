#pragma once

#include "core.hpp"
#include "resourceManager.hpp"

namespace nimbus
{

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

class Mesh
{
   public:
    Mesh() = default;
    
    // with verticies, indicies, and textures
    Mesh(std::vector<Vertex>   verticies,
         std::vector<uint32_t> indicies,
         std::vector<Texture*> textures,
         bool                  normalize = false);

    // with verticies and textures
    Mesh(std::vector<Vertex>   verticies,
         std::vector<Texture*> textures,
         bool                  normalize = false);

    // with verticies and indicies
    Mesh(std::vector<Vertex>   verticies,
         std::vector<uint32_t> indicies,
         bool                  normalize = false);

    // with verticies only
    Mesh(std::vector<Vertex> verticies, bool normalize = false);

    void setShader(Shader* p_shader);

    const Shader* getShader() const;

    void draw() const;

   private:
    std::vector<Vertex>   m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<Texture*> m_textures;
    Shader*               mp_shader = nullptr;
    bool                  m_normalize;
    // render data
    uint32_t m_vao;
    uint32_t m_vbo;
    uint32_t m_ebo;
    bool     m_hasEbo = false;

    void _setupMesh();
};

}  // namespace nimbus