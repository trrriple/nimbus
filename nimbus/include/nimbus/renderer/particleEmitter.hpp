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
        point = 0,
        circle,
        rectangle,
        line
    };

    struct CircleVolumeParameters
    {
        f32_t radius = 0.1f;
    };

    struct RectVolumeParameters
    {
        f32_t width  = 0.1f;
        f32_t height = 0.1f;
    };

    struct LineVolumeParameters
    {
        f32_t length = 0.1f;
    };

    struct Parameters
    {
        glm::vec3                 centerPosition  = glm::vec3(0.0f);
        SpawnVolumeType           spawnVolumeType = SpawnVolumeType::point;
        CircleVolumeParameters    circleVolumeParams;
        RectVolumeParameters      rectVolumeParams;
        LineVolumeParameters      lineVolumeParams;
        f32_t                     lifetimeMin_s           = 0.5f;
        f32_t                     lifetimeMax_s           = 1.0f;
        f32_t                     initSpeedMin            = 0.25f;
        f32_t                     initSpeedMax            = 0.5f;
        glm::vec3                 accelerationMin         = glm::vec3(0.0f);
        glm::vec3                 accelerationMax         = glm::vec3(0.0f);
        glm::vec2                 initSizeMin             = glm::vec2(0.2f);
        glm::vec2                 initSizeMax             = glm::vec2(0.2f);
        f32_t                     ejectionBaseAngle_rad   = 0.0f;
        f32_t                     ejectionSpreadAngle_rad = 6.2831f;
        std::vector<colorSpec>    colors;
        bool                      persist      = true;
        bool                      shrink       = false;
        GraphicsApi::BlendingMode blendingMode = GraphicsApi::BlendingMode::sourceAlphaAdditive;
    };

    ParticleEmitter() = default;

    ParticleEmitter(u32_t               particleCount,
                    const Parameters&   particleParameters,
                    const ref<Texture>& p_texture,
                    const ref<Shader>&  p_customShader = nullptr,
                    bool                is3d           = false);

    ~ParticleEmitter() = default;

    void updateSpawnTransform(const glm::vec3& spawnTranslation,
                              const glm::vec3& spawnRotation,
                              const glm::vec3& spawnScale);

    void update(f32_t deltaTime);

    void draw();

    bool isDone();

    void reset(bool updateLiving = false);

    void chooseColors(size_t min, size_t max);

    void setColor(u32_t idx, const colorSpec& color);

    void addColor(const colorSpec& color);

    void removeColor(u32_t idx);

    void setPosition(const glm::vec3& centerPosition, bool updateLiving = false);

    void setEjectionAngle(f32_t ejectionBaseAngle_rad, f32_t ejectionSpreadAngle_rad);

    void setPersist(bool persist);

    void setShrink(bool shrink);

    void setLifeTime(f32_t min, f32_t max);

    void setInitSpeed(f32_t min, f32_t max);

    void setInitSize(glm::vec2 min, glm::vec2 max);

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
        glm::vec2 startSize      = glm::vec2(0.0f);
        u32_t     colorIdx       = 0;
        f32_t     startLifetime  = 0.0f;
        f32_t     curLifetime    = 0.0f;

        void resetLifetime(f32_t newLifetime)
        {
            startLifetime = newLifetime;
            curLifetime   = startLifetime;
        }

        bool isDead() const
        {
            return curLifetime <= 0;
        }

        void decreaseLifetime(f32_t deltaTime)
        {
            curLifetime -= deltaTime;
        }

        f32_t getLifePercent()
        {
            return curLifetime / startLifetime;
        }
    };
    ////////////////////////////////////////////////////////////////////////////
    // GPU data unique to each particle
    ////////////////////////////////////////////////////////////////////////////
    inline static const BufferFormat k_instanceVboFormat = {
        {k_shaderVec3, "position", BufferComponent::Type::perInstance, 1},
        {k_shaderVec4, "color", BufferComponent::Type::perInstance, 1},
        {k_shaderVec2, "size", BufferComponent::Type::perInstance, 1},

    };
    struct particleInstanceData
    {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec4 color    = glm::vec4(0.0f);
        glm::vec2 size     = glm::vec2(0.0f);

        void reset(const glm::vec3& newPosition, const glm::vec2& newSize, const glm::vec4& newColor)
        {
            position = newPosition;
            size     = newSize;
            color    = newColor;
        }

        void updatePosition(const glm::vec3& velocity, f32_t deltaTime)
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
    u32_t      m_numParticles;
    u32_t      m_numLiveParticles;
    Parameters m_parameters;
    bool       m_is3d;
    glm::vec3  m_spawnTranslation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3  m_spawnRotation    = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3  m_spawnScale       = glm::vec3(1.0f, 1.0f, 1.0f);

    ////////////////////////////////////////////////////////////////////////////
    // Cluster State
    ////////////////////////////////////////////////////////////////////////////
    ref<Shader>                       mp_shader      = nullptr;
    ref<VertexArray>                  mp_vao         = nullptr;
    ref<Texture>                      mp_texture     = nullptr;
    ref<VertexBuffer>                 mp_instanceVbo = nullptr;
    std::vector<particleAttributes>   m_particleAttributes;    // CPU data
    std::vector<particleInstanceData> m_particleInstanceData;  // GPU data

    // distributions
    std::mt19937                          m_randGen;
    std::uniform_real_distribution<f32_t> m_gpRandDist;
    std::uniform_real_distribution<f32_t> m_lifetimeDist;
    std::uniform_real_distribution<f32_t> m_speedDist;
    std::uniform_real_distribution<f32_t> m_accelDistX;
    std::uniform_real_distribution<f32_t> m_accelDistY;
    std::uniform_real_distribution<f32_t> m_accelDistZ;
    std::uniform_real_distribution<f32_t> m_sizeDistX;
    std::uniform_real_distribution<f32_t> m_sizeDistY;
    std::uniform_real_distribution<f32_t> m_angleDist;
    std::uniform_int_distribution<u32_t>  m_colorIndexDist;

    ////////////////////////////////////////////////////////////////////////////
    // Private helper functions
    ////////////////////////////////////////////////////////////////////////////
    void _respawnParticle(particleAttributes* p_attrib, particleInstanceData* p_instDat);

    u32_t _getRandomColorIdx();

    glm::vec3 _getRandomPositionInVolume();
};
}  // namespace nimbus
