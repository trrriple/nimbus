#include "nimbus/nmpch.hpp"
#include "nimbus/core.hpp"

#include "nimbus/renderer/sprite.hpp"
#include "nimbus/renderer/renderer2D.hpp"

namespace nimbus
{

Sprite::Sprite(ref<Texture>& p_texture) : mp_texture(p_texture)
{
}

void Sprite::draw(const glm::vec2& pos,
                  const glm::vec2& size,
                  const glm::vec4& color,
                  const float      rotation)
{
    glm::mat4 model = glm::mat4(1.0f);
    // first translate (transformations are: scale happens first, then
    // rotation, and then final translation happens; eversed order)
    model = glm::translate(model, glm::vec3(pos, 0.0f));
    // move origin of rotation to center of quad
    model
        = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    // then rotate
    model = glm::rotate(
        model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    // move origin back
    model = glm::translate(model,
                           glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    // last scale
    model = glm::scale(model, glm::vec3(size, 1.0f));

    Renderer2D::s_drawQuad(model, mp_texture, color);
}

}  // namespace nimbus