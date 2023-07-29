#include "nimbus/nmpch.hpp"
#include "nimbus/core.hpp"

#include "nimbus/renderer/particleEmitter.hpp"
#include "nimbus/renderer/renderer.hpp"
#include "nimbus/resourceManager.hpp"
#include "nimbus/renderer/graphicsApi.hpp"

#include "glm.hpp"

namespace nimbus
{

const std::string k_particleVertexShader = R"(
    #version 460 core

    layout (location = 0) in vec2 aBasePos;
    layout (location = 1) in vec2 aTexCoords;

    layout (location = 2) in vec3 aParticlePosition;
    layout (location = 3) in vec4 aParticleColor;
    layout (location = 4) in float aParticleSize;

    layout (location = 0) out vec2 TexCoords;
    layout (location = 1) out vec4 Color;

    uniform mat4 u_model;
    uniform mat4 u_viewProjection;

    void main()
    {
        // Pass the texture coordinates and color to the fragment shader
        TexCoords = aTexCoords;
        Color = aParticleColor;
        
        // Convert aParticlePosition to vec4
        vec4 aParticlePosition4 = vec4(aParticlePosition, 0.0);

        // Create scale matrix
        mat4 scale = mat4(
            aParticleSize, 0.0, 0.0,  0.0,
            0.0, aParticleSize, 0.0,  0.0,
            0.0, 0.0,  aParticleSize, 0.0,
            0.0, 0.0,  0.0,           1.0
        );

        // Convert base position to vec4
        vec4 basePos4 = vec4(aBasePos, 0.0, 1.0);

        // Apply transformations
        vec4 finalPos = (scale * basePos4) + aParticlePosition4;
        gl_Position = u_viewProjection * finalPos;
    }
)";

const std::string k_particleFragmentShader = R"(
    #version 460 core

    layout (location = 0) in vec2 TexCoords;
    layout (location = 1) in vec4 Color;

    out vec4 FragColor;

    uniform sampler2D particleTexture;

    void main()
    {
        // Sample the texture at the given coordinates and multiply it by the 
        // color
        FragColor = texture(particleTexture, TexCoords) * Color;
        if(FragColor.a == 0.0f)
            discard;
    }
)";

////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////
ParticleEmitter::ParticleEmitter(uint32_t            particleCount,
                                 const parameters&   particleParameters,
                                 const ref<Texture>& p_texture,
                                 const ref<Shader>&  p_customShader,
                                 bool                is3d)
    : m_numParticles(particleCount),
      m_numLiveParticles(particleCount),
      m_parameters(particleParameters),
      m_is3d(is3d),
      mp_texture(p_texture),
      m_randGen(std::random_device{}())
{
    NM_CORE_ASSERT(!m_is3d, "3D spaces particles are not supported!");
    NM_CORE_ASSERT(m_numParticles,
                   "Particle Emitter needs at least 1 particle!");
    NM_CORE_ASSERT(m_parameters.colors.size(),
                   "Particle Emitter needs at least 1 color!");

    if (p_customShader != nullptr)
    {
        mp_shader = p_customShader;
    }
    else
    {
        mp_shader
            = ResourceManager::s_get().loadShader("particleDefault",
                                                  k_particleVertexShader,
                                                  k_particleFragmentShader);
    }

    if (!m_is3d)
    {
        // clang-format off
        const std::vector<vertexData2d> vData = 
        {
            // centered around (0, 0)
            // pos               // tex
            {glm::vec2(-0.5f, -0.5f), glm::vec2(0.0f, 0.0f)},  // bottom left
            {glm::vec2( 0.5f, -0.5f), glm::vec2(1.0f, 0.0f)},  // bottom right
            {glm::vec2( 0.5f,  0.5f), glm::vec2(1.0f, 1.0f)},  // top right
            {glm::vec2(-0.5f,  0.5f), glm::vec2(0.0f, 1.0f)}   // top left
        };

        std::vector<uint8_t> perVertexIdx = {
            0, 1, 2,  // first triangle
            2, 3, 0  // second triangle
        };
        // clang-format on

        ////////////////////////////////////////////////////////////////////////
        // Random generator setup
        ////////////////////////////////////////////////////////////////////////
        // general purpose distribution
        m_gpRandDist = std::uniform_real_distribution<float>(0.0f, 1.0f);

        // lifetime distribution
        m_lifetimeDist = std::uniform_real_distribution<float>(
            m_parameters.lifetimeMin_s, m_parameters.lifetimeMax_s);

        // speed ditribution
        m_speedDist = std::uniform_real_distribution<float>(
            m_parameters.initSpeedMin, m_parameters.initSpeedMax);

        // accel distribution
        m_accelDistX = std::uniform_real_distribution<float>(
            m_parameters.accelerationMin.x, m_parameters.accelerationMax.x);

        m_accelDistY = std::uniform_real_distribution<float>(
            m_parameters.accelerationMin.y, m_parameters.accelerationMax.y);

        m_accelDistZ = std::uniform_real_distribution<float>(
            m_parameters.accelerationMin.z, m_parameters.accelerationMax.z);

        // size distribution
        m_sizeDist = std::uniform_real_distribution<float>(
            m_parameters.initSizeMin, m_parameters.initSizeMax);

        // project angle distribution
        m_angleDist = std::uniform_real_distribution<float>(
            m_parameters.ejectionBaseAngle_rad
                - m_parameters.ejectionSpreadAngle_rad / 2,
            m_parameters.ejectionBaseAngle_rad
                + m_parameters.ejectionSpreadAngle_rad / 2);

        // color indexing dist
        m_colorIndexDist = std::uniform_int_distribution<uint32_t>(
            0, m_parameters.colors.size() - 1);

        ////////////////////////////////////////////////////////////////////////
        // GPU Buffers
        ////////////////////////////////////////////////////////////////////////
        mp_vao = VertexArray::s_create();

        ref<VertexBuffer> vertexVbo = VertexBuffer::s_create(
            &vData[0], vData.size() * sizeof(vertexData2d));

        vertexVbo->setFormat(ParticleEmitter::k_vertexVboFormat2d);

        mp_vao->addVertexBuffer(vertexVbo);

        mp_vao->setIndexBuffer(
            IndexBuffer::s_create(&perVertexIdx[0], perVertexIdx.size()));

        // we know how many particles we have, so reserve the memory
        m_particleInstanceData.reserve(m_numParticles);
        m_particleAttributes.reserve(m_numParticles);
        for (uint32_t i = 0; i < m_numParticles; i++)
        {
            // set CPU data
            float angle    = m_angleDist(m_randGen);
            float speed    = m_speedDist(m_randGen);
            float lifetime = m_lifetimeDist(m_randGen);
            float size     = m_sizeDist(m_randGen);

            glm::vec3 accel(m_accelDistX(m_randGen),
                            m_accelDistY(m_randGen),
                            m_accelDistZ(m_randGen));

            glm::vec3 positionOffset = _getRandomPositionInVolume();

            glm::vec3 velocity(
                std::sin(angle) * speed, std::cos(angle) * speed, 0.0f);

            glm::vec4 color = _getRandomColorInRange();

            particleAttributes partAtt = {positionOffset,
                                          velocity,
                                          accel,
                                          color,
                                          size,
                                          lifetime,
                                          lifetime,
                                          false,
                                          false};

            m_particleAttributes.push_back(partAtt);

            // set GPU data
            particleInstanceData partInst
                = {m_parameters.centerPosition + positionOffset, color, size};

            m_particleInstanceData.push_back(partInst);
        }

        mp_instanceVbo = VertexBuffer::s_create(
            &m_particleInstanceData[0],
            m_particleInstanceData.size() * sizeof(particleInstanceData),
            VertexBuffer::Type::STREAM_DRAW);

        mp_instanceVbo->setFormat(k_instanceVboFormat);

        mp_vao->addVertexBuffer(mp_instanceVbo);
    }
}

void ParticleEmitter::update(float deltaTime)
{
    NM_PROFILE_DETAIL();

    for (uint32_t i = 0; i < m_numLiveParticles; ++i)
    {
        particleInstanceData* p_instDat = &m_particleInstanceData[i];
        particleAttributes*   p_attrib  = &m_particleAttributes[i];

        p_attrib->decreaseLifetime(deltaTime);

        if (p_attrib->isDead())
        {
            if (!m_parameters.persist)
            {
                ////////////////////////////////////////////////////////////////
                //  Move dead particles to end
                ////////////////////////////////////////////////////////////////
                if (i != m_numLiveParticles - 1)  // prevent swap with itself
                {
                    std::swap(m_particleAttributes[i],
                              m_particleAttributes[m_numLiveParticles - 1]);
                    std::swap(m_particleInstanceData[i],
                              m_particleInstanceData[m_numLiveParticles - 1]);
                }

                m_numLiveParticles -= 1;
            }
            else
            {
                _respawnParticle(p_attrib, p_instDat);
            }
        }
        else
        {
            ////////////////////////////////////////////////////////////////////
            //  Step living particles
            ////////////////////////////////////////////////////////////////////
            float currentLifeLeft = p_attrib->getLifePercent();
            // calculate velocty based on acceleration and lifetime
            glm::vec3 velocity
                = p_attrib->velocity
                  + (p_attrib->acceleration
                     * (p_attrib->startLifetime - p_attrib->curLifetime));

            // position update
            p_instDat->updatePosition(velocity, deltaTime);

            // fade out particles as they age by adjusting alpha if desired
            if (m_parameters.fade)
            {
                // todo fix this to look at start alpha in attributes
                p_instDat->color.a = p_attrib->color.a * currentLifeLeft;
            }

            //  shrink particles as they age if desired
            if (m_parameters.shrink)
            {
                // shrink at a slower rate initially then speed up as
                // particle ages
                float newSize = std::sqrt(currentLifeLeft) * p_attrib->size;

                p_instDat->size = newSize;
            }
        }
    }
}

void ParticleEmitter::draw()
{
    NM_PROFILE();

    if (m_numLiveParticles == 0)
    {
        // if this guy is done emitting don't do anything
        return;
    }

    mp_texture->bind(0);
    mp_shader->bind();
    mp_shader->setInt("particleTexture", 0);

    mp_instanceVbo->bind();
    mp_instanceVbo->setData(&m_particleInstanceData[0],
                            m_numLiveParticles * sizeof(particleInstanceData));

    GraphicsApi::BlendingMode currBlendMode = GraphicsApi::getBlendingMode();
    GraphicsApi::setBlendingMode(m_parameters.blendingMode);

    Renderer::s_submitInstanced(mp_shader, mp_vao, m_numLiveParticles);

    GraphicsApi::setBlendingMode(currBlendMode);
}

bool ParticleEmitter::isDone()
{
    return m_numLiveParticles == 0;
}

void ParticleEmitter::trigger()
{
    // if this is the first time we've been triggered, reset locations on
    // living particles. If not, don't.
    reset(!m_beenTriggered);
}

void ParticleEmitter::reset(bool updateLiving)
{
    NM_PROFILE_DETAIL();

    for (uint32_t i = 0; i < m_numParticles; ++i)
    {
        bool isDead = m_particleAttributes[i].isDead();
        if (updateLiving || isDead)
        {
            if (isDead)
            {
                // only increment for particles we respawn that are dead
                m_numLiveParticles++;
            }
            _respawnParticle(&m_particleAttributes[i],
                             &m_particleInstanceData[i]);
        }
    }
}

void ParticleEmitter::chooseColors(size_t min, size_t max, bool updateLiving)
{
    NM_PROFILE_DETAIL();

    size_t minC = std::clamp(min, size_t(0), m_parameters.colors.size() - 1);
    size_t maxC = std::clamp(max, size_t(0), m_parameters.colors.size() - 1);

    if (minC != min || maxC != max)
    {
        Log::coreError(
            "Particle color range given %i - %i outside of valid range %i - "
            "%i",
            min,
            max,
            0,
            m_parameters.colors.size() - 1);
    }

    m_colorIndexDist = std::uniform_int_distribution<uint32_t>(minC, maxC);

    for (uint32_t i = 0; i < m_numParticles; ++i)
    {
        if (updateLiving && !m_particleAttributes[i].isDead())
        {
            glm::vec4 newColor = _getRandomColorInRange();

            m_particleAttributes[i].color   = newColor;
            m_particleInstanceData[i].color = newColor;
        }
        else
        {
            m_particleAttributes[i].updateColor = true;
        }
    }
}

void ParticleEmitter::setPosition(const glm::vec3& centerPosition,
                                  bool             updateLiving)
{
    m_parameters.centerPosition = centerPosition;

    if (updateLiving)
    {
        for (uint32_t i = 0; i < m_numParticles; ++i)
        {
            m_particleInstanceData[i].position
                = m_parameters.centerPosition
                  + m_particleAttributes[i].positionOffset;
        }
    }
}

void ParticleEmitter::setAngle(float ejectionBaseAngle_rad,
                               float ejectionSpreadAngle_rad,
                               bool  updateLiving)
{
    m_parameters.ejectionBaseAngle_rad   = ejectionBaseAngle_rad;
    m_parameters.ejectionSpreadAngle_rad = ejectionSpreadAngle_rad;

    m_angleDist = std::uniform_real_distribution<float>(
        m_parameters.ejectionBaseAngle_rad
            - m_parameters.ejectionSpreadAngle_rad / 2,
        m_parameters.ejectionBaseAngle_rad
            + m_parameters.ejectionSpreadAngle_rad / 2);

    for (uint32_t i = 0; i < m_numParticles; ++i)
    {
        if (updateLiving && !m_particleAttributes[i].isDead())
        {
            float angle = m_angleDist(m_randGen);
            float speed = m_speedDist(m_randGen);

            m_particleAttributes[i].velocity = glm::vec3(
                std::sin(angle) * speed, std::cos(angle) * speed, 0.0f);
        }
        else
        {
            m_particleAttributes[i].updateVelocity = true;
        }
    }
}

void ParticleEmitter::setPersist(bool persist)
{
    // if we weren't already persisting reset all dead particles
    if (!m_parameters.persist)
    {
        reset(false);
    }
    m_parameters.persist = persist;
}

////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////
void ParticleEmitter::_respawnParticle(particleAttributes*   p_attrib,
                                       particleInstanceData* p_instDat)
{
    if (p_attrib->updateVelocity)
    {
        float angle = m_angleDist(m_randGen);
        float speed = m_speedDist(m_randGen);

        p_attrib->velocity
            = glm::vec3(std::sin(angle) * speed, std::cos(angle) * speed, 0.0f);

        p_attrib->updateVelocity = false;
    }

    if (p_attrib->updateColor)
    {
        glm::vec4 newColor = _getRandomColorInRange();

        p_attrib->color = newColor;

        p_attrib->updateColor = false;
    }

    // always use a random new lifetime to avoid stale behavior
    p_attrib->resetLifetime(m_lifetimeDist(m_randGen));

    // reset the instance data
    p_instDat->reset(m_parameters.centerPosition + p_attrib->positionOffset,
                     p_attrib->size,
                     p_attrib->color);
}

glm::vec4 ParticleEmitter::_getRandomColorInRange()
{
    uint32_t   colorIndex = m_colorIndexDist(m_randGen);
    glm::vec4& min        = m_parameters.colors[colorIndex].colorMin;
    glm::vec4& max        = m_parameters.colors[colorIndex].colorMax;

    float r = min.r + (m_gpRandDist(m_randGen) * (max.r - min.r));
    float g = min.g + (m_gpRandDist(m_randGen) * (max.g - min.g));
    float b = min.b + (m_gpRandDist(m_randGen) * (max.b - min.b));
    float a = min.a + (m_gpRandDist(m_randGen) * (max.a - min.a));

    return {r, g, b, a};
}

glm::vec3 ParticleEmitter::_getRandomPositionInVolume()
{
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    switch (m_parameters.spawnVolumeType)
    {
        case SpawnVolumeType::POINT:
        {
            // For a point, there's no variation. Return zero offset.
            return glm::vec3(0.0f, 0.0f, 0.0f);
        }
        case SpawnVolumeType::CIRCLE:
        {
            float theta = 2 * 3.1415926f * dist(m_randGen);
            float r     = m_parameters.circleVolumeParams.radius
                      * sqrt(dist(m_randGen));
            return glm::vec3(r * cos(theta), r * sin(theta), 0.0f);
        }
        case SpawnVolumeType::RECTANGLE:
        {
            float x = dist(m_randGen) * m_parameters.rectVolumeParams.width;
            float y = dist(m_randGen) * m_parameters.rectVolumeParams.height;
            return glm::vec3(x, y, 0.0f);
        }
        case SpawnVolumeType::LINE:
        {
            float x = dist(m_randGen) * m_parameters.lineVolumeParams.length;
            return glm::vec3(x, 0.0f, 0.0f);
        }
        default:
        {
            // Handle default case here, if needed
            return glm::vec3(0.0f, 0.0f, 0.0f);
        }
    }
}

}  // namespace nimbus
