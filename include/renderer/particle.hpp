#pragma once
#include "common.hpp"
#include "glm.hpp"
#include "platform/buffer.hpp"

namespace nimbus
{
class Particle
{
   public:
    struct instanceData
    {
        glm::vec3 offsetPosition;
        glm::vec4 color;
    };
    // buffer format to describe the structure
    inline static const BufferFormat k_instanceVboFormat = {
        {k_shaderVec3,
         "offsetPosition",
         BufferComponent::Type::PER_INSTANCE,
         1},
        {k_shaderVec4, "color", BufferComponent::Type::PER_INSTANCE, 1},
    };

    Particle(const glm::vec3& offsetPosition,
             const glm::vec3& velocity,
             const glm::vec4& color,
             float            lifetime);

    void update(float deltaTime);
    bool isDead() const;

    instanceData& getInstanceData()
    {
        return m_instanceData;
    }

   private:
    instanceData m_instanceData;
    glm::vec3    m_velocity;
    float        m_lifetime;

    void _decreaseLifetime(float deltaTime);
};
}  // namespace nimbus
