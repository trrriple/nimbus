#include "renderer/particle.hpp"

#include "core.hpp"
#include "glm.hpp"
#include "nmpch.hpp"

namespace nimbus
{

Particle::Particle(const glm::vec3& offsetPosition,
                   const glm::vec3& velocity,
                   const glm::vec4& color,
                   float            lifetime)
    : m_velocity(velocity), m_lifetime(lifetime)
{
    m_instanceData.offsetPosition = offsetPosition;
    m_instanceData.color          = color;
}

void Particle::update(float deltaTime)
{
    m_instanceData.offsetPosition += m_velocity * deltaTime;
    _decreaseLifetime(deltaTime);
}

bool Particle::isDead() const
{
    return m_lifetime <= 0;
}

void Particle::_decreaseLifetime(float deltaTime)
{
    m_lifetime -= deltaTime;
}

}  // namespace nimbus
