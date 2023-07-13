#pragma once

#include "mesh.hpp"
#include "glm.hpp"
#include "shader.hpp"

#include <string>

namespace nimbus
{
class Sprite
{
   public:
    Sprite(const std::string& textureFileNm,
           const std::string& vertShaderFileNm,
           const std::string& fragShaderFileNm,
           const glm::mat4&   projection);

    void draw(const glm::vec2& pos,
              const glm::vec2& size,
              const glm::vec3& color,
              const float      rotation);

   private:
    scope<Mesh>                 mp_mesh;
    inline static const Shader* p_lastShader = nullptr;
};
}  // namespace nimbus