#include "renderer/particleEmitter.hpp"

#include "nmpch.hpp"

#include "application.hpp"
#include "core.hpp"
#include "glm.hpp"
#include "renderer/renderer.hpp"

namespace nimbus
{
ParticleEmitter::ParticleEmitter(const ref<Shader>&  p_shader,
                                 const ref<Texture>& p_texture,
                                 uint32_t            particleCount,
                                 const parameters&   particleParameters,
                                 bool                is3d)
    : mp_shader(p_shader),
      mp_texture(p_texture),
      m_numParticles(particleCount),
      m_numLiveParticles(particleCount),
      m_parameters(particleParameters),
      m_randGen(std::random_device{}()),
      m_is3d(is3d)
{
    NM_CORE_ASSERT(!m_is3d, "3D spaces particles are not supported!");
    NM_CORE_ASSERT(m_numParticles,
                   "Particle Emitter needs at least 1 particle!");
    NM_CORE_ASSERT(m_parameters.colors.size(),
                   "Particle Emitter needs at least 1 color!");
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
            m_parameters.speedMin, m_parameters.speedMax);

        // size distribution
        m_sizeDist = std::uniform_real_distribution<float>(
            m_parameters.sizeMin, m_parameters.sizeMax);

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
        mp_vao = makeRef<VertexArray>();

        ref<VertexBuffer> vertexVbo = makeRef<VertexBuffer>(
            &vData[0], vData.size() * sizeof(vertexData2d));

        vertexVbo->setFormat(ParticleEmitter::k_vertexVboFormat2d);

        mp_vao->addVertexBuffer(vertexVbo);

        mp_vao->setIndexBuffer(
            makeRef<IndexBuffer>(&perVertexIdx[0], perVertexIdx.size()));

        // we know how many particles we have, so reserve the memory
        m_particleInstanceData.reserve(m_numParticles);
        m_particleAttributes.reserve(m_numParticles);
        for (uint32_t i = 0; i < m_numParticles; i++)
        {
            // set CPU data
            float    angle      = m_angleDist(m_randGen);
            float    speed      = m_speedDist(m_randGen);
            float    lifetime   = m_lifetimeDist(m_randGen);
            float    size       = m_sizeDist(m_randGen);


            glm::vec3 positionOffset = _getRandomPositionInVolume();

            glm::vec3 velocity = glm::vec3(
                std::sin(angle) * speed, std::cos(angle) * speed, 0.0f);

            glm::vec4 color = _getRandomColorInRange();

            m_particleAttributes.emplace_back(
                                              positionOffset,
                                              velocity,
                                              color,
                                              size,
                                              lifetime,
                                              lifetime,
                                              false,
                                              false);

            // set GPU data
            m_particleInstanceData.emplace_back(
                m_parameters.centerPosition + positionOffset, color, size);
        }

        mp_instanceVbo = makeRef<VertexBuffer>(
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
            // calculate velocty based on damping or no
            glm::vec3 velocity = m_parameters.damp
                                     ? p_attrib->velocity * currentLifeLeft
                                     : p_attrib->velocity;

            // position update
            p_instDat->updatePosition(velocity, deltaTime);

            // fade out particles as they age by adjusting alpha if desired
            if (m_parameters.fade)
            {
                // todo fix this to look at start alpha in attributes
                p_instDat->color.a = currentLifeLeft;
            }

            //  shrink particles as they age if desired
            if (m_parameters.shrink)
            {
                // shrink at a slower rate initially then speed up as
                // particle ages
                float newSize
                    = std::sqrt(currentLifeLeft) * p_attrib->startSize;

                p_instDat->size = newSize;
            }
        }
    }

}

void ParticleEmitter::draw()
{
    NM_PROFILE();

    if(m_numLiveParticles == 0)
    {
        // if this guy is done emitting don't do anything 
        return;
    }

    const std::string& uniformNm = mp_texture->getUniformNm(0);
    mp_texture->bind(0);
    mp_shader->use();
    mp_shader->setInt(uniformNm, 0);

    mp_instanceVbo->bind();
    mp_instanceVbo->setData(&m_particleInstanceData[0],
                            m_numLiveParticles * sizeof(particleInstanceData));


    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    Renderer::submitInstanced(mp_shader, mp_vao, m_numLiveParticles);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    Texture::s_unbind();
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

    if(minC != min || maxC != max)
    {
        NM_CORE_ERROR(
            "Particle color range given %i - %i outside of valid range %i - "
            "%i\n",
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
                     p_attrib->startSize,
                     p_attrib->color);
}

glm::vec4 ParticleEmitter::_getRandomColorInRange()
{   
    uint32_t colorIndex = m_colorIndexDist(m_randGen);
    glm::vec4& min = m_parameters.colors[colorIndex].colorMin;
    glm::vec4& max = m_parameters.colors[colorIndex].colorMax;

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
