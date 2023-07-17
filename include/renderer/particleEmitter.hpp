#pragma once
#include <random>
#include <vector>

#include "common.hpp"
#include "glm.hpp"
#include "platform/buffer.hpp"
#include "renderer/shader.hpp"
#include "renderer/texture.hpp"
#include "platform/rendererApi.hpp"

namespace nimbus
{
class ParticleEmitter
{
   public:
    struct colorSpec
    {
        glm::vec4 colorMin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);  // 0 to Full
        glm::vec4 colorMax = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    };

    enum class SpawnVolumeType
    {
        POINT,
        CIRCLE,
        RECTANGLE,
        LINE
    };

    struct circleVolumeParameters
    {
        float radius = 1.0f;
    };

    struct rectVolumeParameters
    {
        float width  = 1.0f;
        float height = 1.0f;
    };

    struct lineVolumeParameters
    {
        float length = 1.0f;
    };

    struct parameters
    {
        glm::vec3                 centerPosition  = glm::vec3(0.0f);
        SpawnVolumeType           spawnVolumeType = SpawnVolumeType::POINT;
        circleVolumeParameters    circleVolumeParams;
        rectVolumeParameters      rectVolumeParams;
        lineVolumeParameters      lineVolumeParams;
        float                     lifetimeMin_s           = 1.0f;
        float                     lifetimeMax_s           = 1.0f;
        float                     speedMin                = 10.0f;
        float                     speedMax                = 10.0f;
        glm::vec3                 accelerationMin         = glm::vec3(0.0f);
        glm::vec3                 accelerationMax         = glm::vec3(0.0f);
        float                     sizeMin                 = 5.0f;
        float                     sizeMax                 = 5.0f;
        float                     ejectionBaseAngle_rad   = 0.0f;
        float                     ejectionSpreadAngle_rad = 6.2831f;
        std::vector<colorSpec>    colors;
        bool                      persist = false;
        bool                      fade    = false;
        bool                      shrink  = false;
        RendererApi::BlendingMode blendingMode
            = RendererApi::BlendingMode::SOURCE_ALPHA_ADDITIVE;
    };

    ParticleEmitter(uint32_t            particleCount,
                    const parameters&   particleParameters,
                    const ref<Texture>& p_texture,
                    const ref<Shader>&  p_customShader = nullptr,
                    bool                is3d = false);

    ~ParticleEmitter() = default;

    void update(float deltaTime);

    void draw();

    bool isDone();

    void trigger();

    void reset(bool updateLiving = false);

    void chooseColors(size_t min, size_t max, bool updateLiving = false);

    void setPosition(const glm::vec3& centerPosition,
                     bool             updateLiving = false);

    void setAngle(float ejectionBaseAngle_rad,
                  float ejectionSpreadAngle_rad,
                  bool  updateLiving = false);

    
    void setPersist(bool persist);

   private:
    ////////////////////////////////////////////////////////////////////////////
    // CPU data unique to each particle
    ////////////////////////////////////////////////////////////////////////////
    struct particleAttributes
    {
        glm::vec3 positionOffset;
        glm::vec3 velocity;
        glm::vec3 acceleration;
        glm::vec4 color;
        float     size;
        float     startLifetime;
        float     curLifetime;
        bool      updateColor;
        bool      updateVelocity;

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
        glm::vec3 position;
        glm::vec4 color;
        float     size;

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
    parameters m_parameters;
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

    glm::vec4 _getRandomColorInRange();

    glm::vec3 _getRandomPositionInVolume();
};
}  // namespace nimbus
