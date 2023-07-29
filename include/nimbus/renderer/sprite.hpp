#pragma once

#include "nimbus/common.hpp"
#include "nimbus/renderer/texture.hpp"

#include <string>

#include "glm.hpp"

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