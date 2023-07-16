#pragma once
#include "common.hpp"
#include "glm.hpp"
#include "platform/buffer.hpp"
#include "renderer/shader.hpp"
#include "renderer/texture.hpp"

#include <vector>
#include <random>

namespace nimbus
{
class ParticleEmitter
{
   public:

    struct colorSpec
    {
        glm::vec4 colorMin;
        glm::vec4 colorMax;
    };

    struct parameters
    {
        uint32_t               particleCount;
        glm::vec3              bound;
        float                  lifetimeMin_s;
        float                  lifetimeMax_s;
        float                  speedMin;
        float                  speedMax;
        float                  sizeMin;
        float                  sizeMax;
        float                  baseAngle_rad;
        float                  spreadAngle_rad;
        std::vector<colorSpec> colors;
        bool                   persist;
        bool                   fade;
        bool                   damp;
        bool                   shrink;
    };

    ParticleEmitter(const ref<Shader>&  p_shader,
                    const ref<Texture>& p_texture,
                    const parameters&   particleParameters,
                    const glm::vec3&    centerPosition,
                    bool                is3d = false);



    ~ParticleEmitter() = default;

    void update(float deltaTime);

    void update(float deltaTime, const glm::vec3& centerPosition);

    void draw();

    void chooseColor(size_t min, size_t max);

    void setAngle(float baseAngle_rad, float spreadAngle_rad);

   private:
    ////////////////////////////////////////////////////////////////////////////
    // CPU data unique to each particle
    ////////////////////////////////////////////////////////////////////////////
    struct particleAttributes
    {
        glm::vec3 velocity;
        float     startSize;
        float     startLifetime;
        float     curLifetime;

        void setVelocity(const glm::vec3& newVelocity)
        {
            velocity = newVelocity;
        }
        bool isDead() const
        {
            return curLifetime <= 0;
        }
        void decreaseLifetime(float deltaTime)
        {
            curLifetime -= deltaTime;
        }
        void reset(float newLifetime, float newSize)
        {
            startLifetime = newLifetime;
            curLifetime   = startLifetime;
            startSize     = newSize;
        }
        float getLifePercent()
        {
            return curLifetime / startLifetime;
        }
    };
    std::vector<particleAttributes> m_particleAttributes;
    ////////////////////////////////////////////////////////////////////////////
    // GPU data unique to each particle
    ////////////////////////////////////////////////////////////////////////////
    inline static const BufferFormat k_instanceVboFormat = {
        {k_shaderVec3,
         "offsetPosition",
         BufferComponent::Type::PER_INSTANCE,
         1},
        {k_shaderVec4, "color", BufferComponent::Type::PER_INSTANCE, 1},
        {k_shaderFloat, "size", BufferComponent::Type::PER_INSTANCE, 1},

    };
    struct particleInstanceData
    {
        glm::vec3 offsetPosition;
        glm::vec4 color;
        float     size;

        void updatePosition(const glm::vec3& velocity, float deltaTime)
        {
            offsetPosition += velocity * deltaTime;
        }
        void updateAlpha(float alpha)
        {
            color.a = alpha;
        }
        void updateColor(const glm::vec4& newColor)
        {
            color = newColor;
        }
        void updateSize(float newSize)
        {
            size = newSize;
        }
        void reset(const glm::vec3  newPosition,
                   float            newSize,
                   const glm::vec4& newColor)
        {
            offsetPosition = newPosition;
            size           = newSize;
            color          = newColor;
        }
    };
    std::vector<particleInstanceData> m_particleInstanceData;
    
    ////////////////////////////////////////////////////////////////////////////
    // GPU data shared by all particles
    ////////////////////////////////////////////////////////////////////////////
    inline static const BufferFormat k_vertexVboFormat3d = {
        {k_shaderVec3, "vertexPosition"},
        {k_shaderVec2, "texCoords"},
    };

    inline static const BufferFormat k_vertexVboFormat2d = {
        {k_shaderVec2, "vertexPosition"},
        {k_shaderVec2, "texCoords"},
    };

    struct vertexData3d
    {
        glm::vec3 vertexPosition;
        glm::vec2 texCoords;
    };
    struct vertexData2d
    {
        glm::vec2 vertexPosition;
        glm::vec2 texCoords;
    };

    ref<Shader>       mp_shader      = nullptr;
    ref<VertexArray>  mp_vao         = nullptr;
    ref<Texture>      mp_texture     = nullptr;
    ref<VertexBuffer> mp_instanceVbo = nullptr;

    // parameters
    parameters m_parameters;
    
    // cluster state
    glm::vec3 m_centerPosition;
    uint32_t  m_numLiveParticles;

    std::mt19937                            m_randGen;
    std::uniform_real_distribution<float>   m_gpRandDist;
    std::uniform_real_distribution<float>   m_lifetimeDist;
    std::uniform_real_distribution<float>   m_speedDist;
    std::uniform_real_distribution<float>   m_sizeDist;
    std::uniform_real_distribution<float>   m_angleDist;
    std::uniform_int_distribution<uint32_t> m_colorIndexDist;

    bool m_is3d;

    glm::vec4 _getRandomColorInRange(const glm::vec4& min,
                                     const glm::vec4& max);
};
}  // namespace nimbus
