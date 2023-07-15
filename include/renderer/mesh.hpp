#pragma once

#include <string>

#include "common.hpp"
#include "glm.hpp"
#include "platform/buffer.hpp"
#include "renderer/shader.hpp"
#include "renderer/texture.hpp"

namespace nimbus
{

class Mesh
{
   public:
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
        glm::vec3 tangent;
        glm::vec3 bitangent;
    };

    // buffer format to describe the structure
    inline static const BufferFormat k_vboFormat = {
        {k_shaderVec3, "position"},
        {k_shaderVec3, "normal"},
        {k_shaderVec2, "texCoords"},
        {k_shaderVec3, "tangent"},
        {k_shaderVec3, "bitangent"},
    };

    inline static const BufferFormat k_vboFormatNormalize = {
        {k_shaderVec3, "position", true},
        {k_shaderVec3, "normal", true},
        {k_shaderVec2, "texCoords", true},
        {k_shaderVec3, "tangent", true},
        {k_shaderVec3, "bitangent", true},
    };

    // with verticies, indicies, and textures
    Mesh(std::vector<Vertex>        verticies,
         std::vector<uint32_t>      indicies,
         std::vector<ref<Texture>> textures,
         bool                       normalize = false);

    // with verticies and textures
    Mesh(std::vector<Vertex>        verticies,
         std::vector<ref<Texture>> textures,
         bool                       normalize = false);

    // with verticies and indicies
    Mesh(std::vector<Vertex>   verticies,
         std::vector<uint32_t> indicies,
         bool                  normalize = false);

    // with verticies only
    Mesh(std::vector<Vertex> verticies, bool normalize = false);

    void setShader(ref<Shader>& p_shader);

    ref<Shader>& getShader();

    void draw(glm::mat4& model) const;

   private:
    std::vector<Vertex>        m_vertices;
    std::vector<uint32_t>      m_indices;
    std::vector<ref<Texture>>  m_textures;
    bool                       m_normalize;
    ref<Shader>                mp_shader = nullptr;
    ref<VertexBuffer>          mp_vbo    = nullptr;
    ref<VertexArray>           mp_vao    = nullptr;
    ref<IndexBuffer>           mp_ebo    = nullptr;

    void _setupMesh();
};

}  // namespace nimbus