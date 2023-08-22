#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/renderer/particleEmitter.hpp"
#include "nimbus/renderer/renderer.hpp"
#include "nimbus/core/application.hpp"
#include "nimbus/core/resourceManager.hpp"
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
    layout (location = 4) in vec3 aParticleSize;

    layout (location = 0) out vec2 TexCoords;
    layout (location = 1) out vec4 Color;

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
            aParticleSize.x, 0.0, 0.0,  0.0,
            0.0, aParticleSize.y, 0.0,  0.0,
            0.0, 0.0,             1.0,  0.0,
            0.0, 0.0,             0.0,  1.0
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ParticleEmitter::ParticleEmitter(u32_t               particleCount,
                                 const Parameters&   particleParameters,
                                 const ref<Texture>& p_texture,
                                 const ref<Shader>&  p_customShader,
                                 bool                is3d)
    : m_numParticles(particleCount),
      m_numLiveParticles(particleCount),  // technically not
      m_parameters(particleParameters),
      m_is3d(is3d),
      mp_texture(p_texture),
      m_randGen(std::random_device{}())
{
    NB_CORE_ASSERT(!m_is3d, "3D spaces particles are not supported!");
    NB_CORE_ASSERT(m_numParticles, "Particle Emitter needs at least 1 particle!");
    NB_CORE_ASSERT(m_parameters.colors.size(), "Particle Emitter needs at least 1 color!");

    if (p_customShader != nullptr)
    {
        mp_shader = p_customShader;
    }
    else
    {
        mp_shader = Application::s_get().getResourceManager().loadShader("particleDefault",
                                                                         k_particleVertexShader,
                                                                         k_particleFragmentShader);
    }

    if (mp_texture == nullptr)
    {
        // if no texture applied, assume white
        mp_texture = Renderer::getWhiteTexture();
    }

    if (!m_is3d)
    {
        // clang-format off
        const std::vector<vertexData2d> vData = 
        {
            // centered around (0, 0)
            // pos               // tex
            {glm::vec2(-0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},  // bottom left
            {glm::vec2( 0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},  // bottom right
            {glm::vec2( 0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},  // top right
            {glm::vec2(-0.5f,  0.5f), glm::vec2(0.0f, 0.0f)}   // top left
        };

        std::vector<u8_t> perVertexIdx = {
            0, 1, 2,  // first triangle
            2, 3, 0  // second triangle
        };
        // clang-format on

        ////////////////////////////////////////////////////////////////////////
        // Random generator setup
        ////////////////////////////////////////////////////////////////////////
        // general purpose distribution
        m_gpRandDist = std::uniform_real_distribution<f32_t>(0.0f, 1.0f);

        // lifetime distribution
        setLifeTime(m_parameters.lifetimeMin_s, m_parameters.lifetimeMax_s);

        // speed ditribution
        setInitSpeed(m_parameters.initSpeedMin, m_parameters.initSpeedMax);

        // accel distribution
        setAcceleration(m_parameters.accelerationMin, m_parameters.accelerationMax);

        // size distribution
        setInitSize(m_parameters.initSizeMin, m_parameters.initSizeMax);

        setEjectionAngle(m_parameters.ejectionBaseAngle_rad, m_parameters.ejectionSpreadAngle_rad);

        chooseColors(0, m_parameters.colors.size() - 1);

        ////////////////////////////////////////////////////////////////////////
        // GPU Buffers
        ////////////////////////////////////////////////////////////////////////
        mp_vao = VertexArray::s_create();

        ref<VertexBuffer> vertexVbo = VertexBuffer::s_create(&vData[0], vData.size() * sizeof(vertexData2d));

        vertexVbo->setFormat(ParticleEmitter::k_vertexVboFormat2d);

        mp_vao->addVertexBuffer(vertexVbo);

        mp_vao->setIndexBuffer(IndexBuffer::s_create(&perVertexIdx[0], perVertexIdx.size()));

        // we know how many particles we have, so reserve the memory
        m_particleInstanceData.reserve(m_numParticles);
        m_particleAttributes.reserve(m_numParticles);
        for (u32_t i = 0; i < m_numParticles; i++)
        {
            glm::vec3 positionOffset = _getRandomPositionInVolume();

            particleAttributes partAtt
                = {positionOffset, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, 0, 1.0, 0.0};

            m_particleAttributes.push_back(partAtt);

            // set GPU data
            particleInstanceData partInst
                = {m_parameters.centerPosition + positionOffset, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}};

            m_particleInstanceData.push_back(partInst);
        }

        mp_instanceVbo = VertexBuffer::s_create(&m_particleInstanceData[0],
                                                m_particleInstanceData.size() * sizeof(particleInstanceData),
                                                VertexBuffer::Type::streamDraw);

        mp_instanceVbo->setFormat(k_instanceVboFormat);

        mp_vao->addVertexBuffer(mp_instanceVbo);
    }
}

void ParticleEmitter::updateSpawnTransform(const glm::vec3& spawnTranslation,
                                           const glm::vec3& spawnRotation,
                                           const glm::vec3& spawnScale)
{
    m_spawnTranslation = spawnTranslation;
    m_spawnRotation    = spawnRotation;
    m_spawnScale       = spawnScale;
}

void ParticleEmitter::update(f32_t deltaTime)
{
    NB_PROFILE_DETAIL();

    for (u32_t i = 0; i < m_numLiveParticles; ++i)
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
                    std::swap(m_particleAttributes[i], m_particleAttributes[m_numLiveParticles - 1]);
                    std::swap(m_particleInstanceData[i], m_particleInstanceData[m_numLiveParticles - 1]);
                }

                // this is important, we don't want to visit dead particles
                // during this current loop, so indeed modify the loop
                // condition
                m_numLiveParticles--;

                continue;
            }
            else
            {
                // don't adjust m_numLiveParticles, just respawn this particle
                _respawnParticle(p_attrib, p_instDat);
            }
        }

        ////////////////////////////////////////////////////////////////////
        //  Step living particles
        ////////////////////////////////////////////////////////////////////
        f32_t currentLifeLeft = p_attrib->getLifePercent();
        // calculate velocity based on acceleration and lifetime
        glm::vec3 velocity
            = p_attrib->velocity + (p_attrib->acceleration * (p_attrib->startLifetime - p_attrib->curLifetime));

        // position update
        p_instDat->updatePosition(velocity, deltaTime);

        // update color
        p_instDat->color = glm::mix(m_parameters.colors[p_attrib->colorIdx].colorEnd,
                                    m_parameters.colors[p_attrib->colorIdx].colorStart,
                                    currentLifeLeft);

        //  shrink particles as they age if desired
        if (m_parameters.shrink)
        {
            // shrink at a slower rate initially then speed up as
            // particle ages
            f32_t sizeScalar = std::sqrt(currentLifeLeft);

            p_instDat->size.x = sizeScalar * p_attrib->startSize.x;
            p_instDat->size.y = sizeScalar * p_attrib->startSize.y;
        }
    }
}

void ParticleEmitter::draw()
{
    NB_PROFILE();

    if (m_numLiveParticles == 0)
    {
        // if this guy is done emitting don't do anything
        return;
    }

    // if these aren't loaded, don't proceed
    if(!mp_texture->bind(0))
    {
        return;
    }
    
    if(!mp_shader->bind())
    {
        return;
    }

    mp_shader->setInt("particleTexture", 0);

    mp_instanceVbo->bind();
    mp_instanceVbo->setData(&m_particleInstanceData[0], m_numLiveParticles * sizeof(particleInstanceData));

    GraphicsApi::BlendingMode currBlendMode = GraphicsApi::getBlendingMode();
    GraphicsApi::setBlendingMode(m_parameters.blendingMode);

    Renderer::s_renderInstanced(mp_shader, mp_vao, m_numLiveParticles);

    GraphicsApi::setBlendingMode(currBlendMode);
}

bool ParticleEmitter::isDone()
{
    return m_numLiveParticles == 0;
}

void ParticleEmitter::reset(bool updateLiving)
{
    NB_PROFILE_DETAIL();

    for (u32_t i = 0; i < m_numParticles; ++i)
    {
        bool isDead = m_particleAttributes[i].isDead();
        if (updateLiving || isDead)
        {
            if (isDead)
            {
                // only increment for particles we respawn that are dead
                m_numLiveParticles++;
            }
            _respawnParticle(&m_particleAttributes[i], &m_particleInstanceData[i]);
        }
    }
}

void ParticleEmitter::chooseColors(size_t min, size_t max)
{
    NB_PROFILE_DETAIL();

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

    m_colorIndexDist = std::uniform_int_distribution<u32_t>(minC, maxC);
}

void ParticleEmitter::setColor(u32_t idx, const colorSpec& color)
{
    if (idx >= m_parameters.colors.size())
    {
        Log::coreError("Error: color index out of range. Have %i colors", m_parameters.colors.size());
        return;
    }

    m_parameters.colors[idx] = color;
}

void ParticleEmitter::addColor(const colorSpec& color)
{
    m_parameters.colors.push_back(color);
}

void ParticleEmitter::removeColor(u32_t idx)
{
    if (idx >= m_parameters.colors.size())
    {
        Log::coreError("Error: color index out of range. Have %i colors", m_parameters.colors.size());
        return;
    }

    m_parameters.colors.erase(m_parameters.colors.begin() + idx);
}

void ParticleEmitter::setPosition(const glm::vec3& centerPosition, bool updateLiving)
{
    m_parameters.centerPosition = centerPosition;

    if (updateLiving)
    {
        for (u32_t i = 0; i < m_numParticles; ++i)
        {
            m_particleInstanceData[i].position = m_parameters.centerPosition + m_particleAttributes[i].positionOffset;
        }
    }
}

void ParticleEmitter::setEjectionAngle(f32_t ejectionBaseAngle_rad, f32_t ejectionSpreadAngle_rad)
{
    m_parameters.ejectionBaseAngle_rad   = ejectionBaseAngle_rad;
    m_parameters.ejectionSpreadAngle_rad = ejectionSpreadAngle_rad;

    m_angleDist = std::uniform_real_distribution<f32_t>(
        (m_parameters.ejectionBaseAngle_rad - m_parameters.ejectionSpreadAngle_rad / 2),
        (m_parameters.ejectionBaseAngle_rad + m_parameters.ejectionSpreadAngle_rad / 2));
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

void ParticleEmitter::setShrink(bool shrink)
{
    m_parameters.shrink = shrink;
}

void ParticleEmitter::setLifeTime(f32_t min, f32_t max)
{
    NB_CORE_ASSERT(min <= max, "Lifetime min must be <= max");

    m_parameters.lifetimeMin_s = min;
    m_parameters.lifetimeMax_s = max;
    m_lifetimeDist = std::uniform_real_distribution<f32_t>(m_parameters.lifetimeMin_s, m_parameters.lifetimeMax_s);
}

void ParticleEmitter::setInitSpeed(f32_t min, f32_t max)
{
    NB_CORE_ASSERT(min <= max, "Init Speed min must be <= max");

    m_parameters.initSpeedMin = min;
    m_parameters.initSpeedMax = max;
    m_speedDist = std::uniform_real_distribution<f32_t>(m_parameters.initSpeedMin, m_parameters.initSpeedMax);
}

void ParticleEmitter::setInitSize(glm::vec2 min, glm::vec2 max)
{
    NB_CORE_ASSERT(min.x <= max.x, "Size x min must be <= max");
    NB_CORE_ASSERT(min.y <= max.y, "Size y min must be <= max");

    m_parameters.initSizeMin = min;
    m_parameters.initSizeMax = max;
    m_sizeDistX = std::uniform_real_distribution<f32_t>(m_parameters.initSizeMin.x, m_parameters.initSizeMax.x);

    m_sizeDistY = std::uniform_real_distribution<f32_t>(m_parameters.initSizeMin.y, m_parameters.initSizeMax.y);
}

void ParticleEmitter::setAcceleration(glm::vec3 min, glm::vec3 max)
{
    NB_CORE_ASSERT(min.x <= max.x, "Accel x min must be <= max");
    NB_CORE_ASSERT(min.y <= max.y, "Accel y min must be <= max");
    NB_CORE_ASSERT(min.z <= max.z, "Accel z min must be <= max");

    m_parameters.accelerationMin = min;
    m_parameters.accelerationMax = max;

    m_accelDistX
        = std::uniform_real_distribution<f32_t>(m_parameters.accelerationMin.x, m_parameters.accelerationMax.x);

    m_accelDistY
        = std::uniform_real_distribution<f32_t>(m_parameters.accelerationMin.y, m_parameters.accelerationMax.y);

    m_accelDistZ
        = std::uniform_real_distribution<f32_t>(m_parameters.accelerationMin.z, m_parameters.accelerationMax.z);
}

void ParticleEmitter::setBlendMode(GraphicsApi::BlendingMode mode)
{
    m_parameters.blendingMode = mode;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ParticleEmitter::_respawnParticle(particleAttributes* p_attrib, particleInstanceData* p_instDat)
{
    p_attrib->resetLifetime(m_lifetimeDist(m_randGen));

    f32_t angle = -m_angleDist(m_randGen) - m_spawnRotation.z;
    f32_t speed = m_speedDist(m_randGen);

    p_attrib->velocity = glm::vec3(std::sin(angle) * speed, std::cos(angle) * speed, 0.0f);

    p_attrib->colorIdx = _getRandomColorIdx();

    p_attrib->startSize.x = m_sizeDistX(m_randGen) * m_spawnScale.x;
    p_attrib->startSize.y = m_sizeDistY(m_randGen) * m_spawnScale.y;

    p_attrib->acceleration.x = m_accelDistX(m_randGen);
    p_attrib->acceleration.y = m_accelDistY(m_randGen);
    p_attrib->acceleration.z = m_accelDistZ(m_randGen);

    // reset the instance data
    p_instDat->reset(m_parameters.centerPosition + p_attrib->positionOffset + m_spawnTranslation,
                     p_attrib->startSize,
                     m_parameters.colors[p_attrib->colorIdx].colorStart);
}

u32_t ParticleEmitter::_getRandomColorIdx()
{
    return m_colorIndexDist(m_randGen);
}

glm::vec3 ParticleEmitter::_getRandomPositionInVolume()
{
    std::uniform_real_distribution<f32_t> dist(-1.0f, 1.0f);

    switch (m_parameters.spawnVolumeType)
    {
        case SpawnVolumeType::point:
        {
            // For a point, there's no variation. Return zero offset.
            return glm::vec3(0.0f, 0.0f, 0.0f);
        }
        case SpawnVolumeType::circle:
        {
            f32_t theta = 2 * 3.1415926f * dist(m_randGen);
            f32_t r     = m_parameters.circleVolumeParams.radius * sqrt(dist(m_randGen));
            return glm::vec3(r * cos(theta), r * sin(theta), 0.0f);
        }
        case SpawnVolumeType::rectangle:
        {
            f32_t x = dist(m_randGen) * m_parameters.rectVolumeParams.width;
            f32_t y = dist(m_randGen) * m_parameters.rectVolumeParams.height;
            return glm::vec3(x, y, 0.0f);
        }
        case SpawnVolumeType::line:
        {
            f32_t x = dist(m_randGen) * m_parameters.lineVolumeParams.length;
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
