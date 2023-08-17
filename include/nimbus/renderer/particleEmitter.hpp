#pragma once

#include "nimbus/core/common.hpp"
#include "nimbus/renderer/buffer.hpp"
#include "nimbus/renderer/shader.hpp"
#include "nimbus/renderer/texture.hpp"
#include "nimbus/renderer/graphicsApi.hpp"

#include <random>
#include <vector>

#include "glm.hpp"

namespace nimbus
{
class ParticleEmitter : public refCounted
{
   public:
    struct colorSpec
    {
        glm::vec4 colorStart = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glm::vec4 colorEnd   = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    };

    enum class SpawnVolumeType
    {
        POINT = 0,
        CIRCLE,
        RECTANGLE,
        LINE
    };

    struct CircleVolumeParameters
    {
        float radius = 0.1f;
    };

    struct RectVolumeParameters
    {
        float width  = 0.1f;
        float height = 0.1f;
    };

    struct LineVolumeParameters
    {
        float length = 0.1f;
    };

    struct Parameters
    {
        glm::vec3                 centerPosition  = glm::vec3(0.0f);
        SpawnVolumeType           spawnVolumeType = SpawnVolumeType::POINT;
        CircleVolumeParameters    circleVolumeParams;
        RectVolumeParameters      rectVolumeParams;
        LineVolumeParameters      lineVolumeParams;
        float                     lifetimeMin_s           = 1.0f;
        float                     lifetimeMax_s           = 1.0f;
        float                     initSpeedMin            = 0.2f;
        float                     initSpeedMax            = 0.2f;
        glm::vec3                 accelerationMin         = glm::vec3(0.0f);
        glm::vec3                 accelerationMax         = glm::vec3(0.0f);
        float                     initSizeMin             = 0.01f;
        float                     initSizeMax             = 0.01f;
        float                     ejectionBaseAngle_rad   = 0.0f;
        float                     ejectionSpreadAngle_rad = 6.2831f;
        std::vector<colorSpec>    colors;
        bool                      persist      = false;
        bool                      staggerStart = true;
        bool                      shrink       = false;
        GraphicsApi::BlendingMode blendingMode
            = GraphicsApi::BlendingMode::SOURCE_ALPHA_ADDITIVE;
    };

    ParticleEmitter() = default;

    ParticleEmitter(uint32_t            particleCount,
                    const Parameters&   particleParameters,
                    const ref<Texture>& p_texture,
                    const ref<Shader>&  p_customShader = nullptr,
                    bool                is3d           = false);

    ~ParticleEmitter() = default;

    void update(float deltaTime);

    void draw(const glm::mat4& transform);

    bool isDone();

    void trigger();

    void reset(bool updateLiving = false);

    void chooseColors(size_t min, size_t max);

    void setColor(uint32_t idx, const colorSpec& color);

    void addColor(const colorSpec& color);

    void removeColor(uint32_t idx);

    void setPosition(const glm::vec3& centerPosition,
                     bool             updateLiving = false);

    void setEjectionAngle(float ejectionBaseAngle_rad,
                          float ejectionSpreadAngle_rad);

    void setPersist(bool persist);

    void setShrink(bool shrink);

    void setLifeTime(float min, float max);

    void setInitSpeed(float min, float max);

    void setInitSize(float min, float max);

    void setAcceleration(glm::vec3 min, glm::vec3 max);

    void setBlendMode(GraphicsApi::BlendingMode mode);

   private:
    ////////////////////////////////////////////////////////////////////////////
    // CPU data unique to each particle
    ////////////////////////////////////////////////////////////////////////////
    struct particleAttributes
    {
        glm::vec3 positionOffset = glm::vec3(0.0f);
        glm::vec3 velocity       = glm::vec3(0.0f);
        glm::vec3 acceleration   = glm::vec3(0.0f);
        uint32_t  colorIdx       = 0;
        float     startSize      = 0.0f;
        float     startLifetime  = 0.0f;
        float     curLifetime    = 0.0f;

        void resetLifetime(float newLifetime)
        {
            startLifetime = newLifetime;
            curLifetime   = startLifetime;
        }

        bool isDead() const
        {
            return curLifetime <= 0;
        }

        void decreaseLifetime(float deltaTime)
        {
            curLifetime -= deltaTime;
        }

        float getLifePercent()
        {
            return curLifetime / startLifetime;
        }
    };
    ////////////////////////////////////////////////////////////////////////////
    // GPU data unique to each particle
    ////////////////////////////////////////////////////////////////////////////
    inline static const BufferFormat k_instanceVboFormat = {
        {k_shaderVec3, "position", BufferComponent::Type::PER_INSTANCE, 1},
        {k_shaderVec4, "color", BufferComponent::Type::PER_INSTANCE, 1},
        {k_shaderFloat, "size", BufferComponent::Type::PER_INSTANCE, 1},

    };
    struct particleInstanceData
    {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec4 color    = glm::vec4(0.0f);
        float     size     = 0.0f;

        void reset(const glm::vec3  newPosition,
                   float            newSize,
                   const glm::vec4& newColor)
        {
            position = newPosition;
            size     = newSize;
            color    = newColor;
        }

        void updatePosition(const glm::vec3& velocity, float deltaTime)
        {
            position += velocity * deltaTime;
        }
    };

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

    ////////////////////////////////////////////////////////////////////////////
    // Cluster parameters
    ////////////////////////////////////////////////////////////////////////////
    uint32_t   m_numParticles;
    uint32_t   m_numLiveParticles;
    Parameters m_parameters;
    bool       m_is3d;

    ////////////////////////////////////////////////////////////////////////////
    // Cluster State
    ////////////////////////////////////////////////////////////////////////////
    bool                              m_beenTriggered = false;
    ref<Shader>                       mp_shader       = nullptr;
    ref<VertexArray>                  mp_vao          = nullptr;
    ref<Texture>                      mp_texture      = nullptr;
    ref<VertexBuffer>                 mp_instanceVbo  = nullptr;
    std::vector<particleAttributes>   m_particleAttributes;    // CPU data
    std::vector<particleInstanceData> m_particleInstanceData;  // GPU data

    std::mt19937                            m_randGen;
    std::uniform_real_distribution<float>   m_gpRandDist;
    std::uniform_real_distribution<float>   m_lifetimeDist;
    std::uniform_real_distribution<float>   m_speedDist;
    std::uniform_real_distribution<float>   m_accelDistX;
    std::uniform_real_distribution<float>   m_accelDistY;
    std::uniform_real_distribution<float>   m_accelDistZ;
    std::uniform_real_distribution<float>   m_sizeDist;
    std::uniform_real_distribution<float>   m_angleDist;
    std::uniform_int_distribution<uint32_t> m_colorIndexDist;

    ////////////////////////////////////////////////////////////////////////////
    // Private helper functions
    ////////////////////////////////////////////////////////////////////////////
    void _respawnParticle(particleAttributes*   p_attrib,
                          particleInstanceData* p_instDat);

    uint32_t _getRandomColorIdx();

    glm::vec3 _getRandomPositionInVolume();
};
}  // namespace nimbus
