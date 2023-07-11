#pragma once

#include "core.hpp"
#include "mesh.hpp"
#include "resourceManager.hpp"

namespace nimbus
{
class Sprite
{
   public:
    Sprite();

    Sprite(const std::string& textureFileNm,
           const std::string& vertShaderFileNm,
           const std::string& fragShaderFileNm,
           const glm::mat4&   projection);

    void draw(const glm::vec2& pos,
              const glm::vec2& size,
              const glm::vec3& color,
              const float      rotation);

   private:
    Mesh                        m_mesh;
    inline static const Shader* p_lastShader = nullptr;
};
}  // namespace nimbus