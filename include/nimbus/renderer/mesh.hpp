#pragma once

#include "nimbus/core/common.hpp"
#include "nimbus/renderer/buffer.hpp"
#include "nimbus/renderer/shader.hpp"
#include "nimbus/renderer/texture.hpp"

#include <string>

#include "glm.hpp"

namespace nimbus
{

class Mesh : public refCounted
{
    inline static const std::string k_texDiffNm = "texDiff";
    inline static const std::string k_texSpecNm = "texSpec";
    inline static const std::string k_texAmbiNm = "texAmbi";
    inline static const std::string k_texNormNm = "texNorm";
    inline static const std::string k_texHghtNm = "texHght";

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
    Mesh(std::vector<Vertex>       verticies,
         std::vector<uint32_t>     indicies,
         std::vector<ref<Texture>> textures,
         bool                      normalize = false);

    // with verticies and textures
    Mesh(std::vector<Vertex> verticies, std::vector<ref<Texture>> textures, bool normalize = false);

    // with verticies and indicies
    Mesh(std::vector<Vertex> verticies, std::vector<uint32_t> indicies, bool normalize = false);

    // with verticies only
    Mesh(std::vector<Vertex> verticies, bool normalize = false);

    void setShader(ref<Shader> p_shader);

    ref<Shader> getShader();

    void draw(const glm::mat4& model) const;

   private:
    std::vector<Vertex>       m_vertices;
    std::vector<uint32_t>     m_indices;
    std::vector<ref<Texture>> m_textures;
    bool                      m_normalize;
    ref<Shader>               mp_shader = nullptr;
    ref<VertexArray>          mp_vao    = nullptr;

    std::vector<std::string> m_texDiffUniformNms;
    std::vector<std::string> m_texSpecUniformNms;
    std::vector<std::string> m_texAmbiUniformNms;
    std::vector<std::string> m_texNormUniformNms;
    std::vector<std::string> m_texHghtUniformNms;

    void _setupMesh();

    void _setupTextureUniforms();
};

}  // namespace nimbus