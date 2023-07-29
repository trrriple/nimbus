#pragma once

#include "glm.hpp"
#include "shader.hpp"
#include "renderer/texture.hpp"

#include <string>

namespace nimbus
{
class Sprite
{
   public:
    Sprite(ref<Texture>&  p_texture);

    void draw(const glm::vec2& pos,
              const glm::vec2& size,
              const glm::vec4& color,
              const float      rotation);

   private:
    ref<Texture>   mp_texture;
};
}  // namespace nimbus