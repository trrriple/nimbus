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
                                 const parameters&   parameters,
                                 const glm::vec3&    centerPosition,
                                 bool                is3d)
    : mp_shader(p_shader),
      mp_texture(p_texture),
      m_parameters(parameters),
      m_centerPosition(centerPosition),
      m_randGen(std::random_device{}()),
      m_is3d(is3d)
{
    NM_CORE_ASSERT(!is3d, "3D spaces particles are not supported!");
    NM_CORE_ASSERT(parameters.particleCount,
                   "Particle Emitter needs at least 1 particle!");
    NM_CORE_ASSERT(parameters.colors.size(),
                   "Particle Emitter needs at least 1 color!");
    if (!is3d)
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
            m_parameters.baseAngle_rad - m_parameters.spreadAngle_rad / 2,
            m_parameters.baseAngle_rad + m_parameters.spreadAngle_rad / 2);
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
        m_particleInstanceData.reserve(m_parameters.particleCount);
        m_particleAttributes.reserve(m_parameters.particleCount);
        for (uint32_t i = 0; i < m_parameters.particleCount; i++)
        {
            // set CPU data
            float    angle      = m_angleDist(m_randGen);
            float    speed      = m_speedDist(m_randGen);
            float    lifetime   = m_lifetimeDist(m_randGen);
            float    size       = m_sizeDist(m_randGen);
            uint32_t colorIndex = m_colorIndexDist(m_randGen);

            m_particleAttributes.emplace_back(
                glm::vec3(
                    std::sin(angle) * speed, std::cos(angle) * speed, 0.0f),
                size,
                lifetime,
                lifetime);

            // set GPU data
            m_particleInstanceData.emplace_back(
                m_centerPosition,
                _getRandomColorInRange(
                    m_parameters.colors[colorIndex].colorMin,
                    m_parameters.colors[colorIndex].colorMax),
                size);
        }

        m_numLiveParticles = m_parameters.particleCount;

        mp_instanceVbo = makeRef<VertexBuffer>(
            &m_particleInstanceData[0],
            m_particleInstanceData.size() * sizeof(particleInstanceData),
            VertexBuffer::Type::STREAM_DRAW);

        mp_instanceVbo->setFormat(k_instanceVboFormat);

        mp_vao->addVertexBuffer(mp_instanceVbo);
    }
}

void ParticleEmitter::update(float deltaTime, const glm::vec3& centerPosition)
{
    m_centerPosition = centerPosition;
    update(deltaTime);
}

void ParticleEmitter::update(float deltaTime)
{
    NM_PROFILE_DETAIL();

    const uint32_t curLiveParticles = m_numLiveParticles;
    for (uint32_t i = 0; i < curLiveParticles; ++i)
    {
        particleInstanceData* p_instDat = &m_particleInstanceData[i];
        particleAttributes*   p_attrib  = &m_particleAttributes[i];

        p_attrib->decreaseLifetime(deltaTime);

        // dead particles are at the end of the vector
        if (p_attrib->isDead())
        {
            if (!m_parameters.persist)
            {
                // we want particles to die off
                std::swap(*p_attrib, m_particleAttributes.back());
                std::swap(*p_instDat, m_particleInstanceData.back());
                --m_numLiveParticles;
            }
            else
            {
                // we want to respawn particles as they die
                // randomly pick a lifetime between 0 and the maximum

                p_attrib->reset(m_lifetimeDist(m_randGen),
                                m_sizeDist(m_randGen));
                                
                float angle = m_angleDist(m_randGen);
                float speed = m_speedDist(m_randGen);

                m_particleAttributes[i].setVelocity(glm::vec3(
                    std::sin(angle) * speed, std::cos(angle) * speed, 0.0f));

                // reset the instance data
                uint32_t colorIndex = m_colorIndexDist(m_randGen);
                p_instDat->reset(m_centerPosition,
                                 p_attrib->startSize,
                                 _getRandomColorInRange(
                                     m_parameters.colors[colorIndex].colorMin,
                                     m_parameters.colors[colorIndex].colorMax));

            }
        }
        else
        {
            glm::vec3 velocity
                = m_parameters.damp
                      ? p_attrib->velocity * p_attrib->getLifePercent()
                      : p_attrib->velocity;

            p_instDat->updatePosition(velocity, deltaTime);

            if (m_parameters.fade)
            {
                p_instDat->updateAlpha(p_attrib->getLifePercent());
            }

            if (m_parameters.shrink)
            {
                // shrink at a slower rate initially then speed up as
                // particle ages
                float newSize = std::sqrt(p_attrib->getLifePercent())
                                * p_attrib->startSize;

                p_instDat->updateSize(newSize);
            }
        }
    }
}

void ParticleEmitter::draw()
{
    NM_PROFILE();

    if (m_numLiveParticles == 0)
    {
        // nothing to draw
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

void ParticleEmitter::chooseColor(size_t min, size_t max)
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


    for (uint32_t i = 0; i < m_numLiveParticles; ++i)
    {
        uint32_t colorIndex = m_colorIndexDist(m_randGen);

        m_particleInstanceData[i].updateColor(
            _getRandomColorInRange(m_parameters.colors[colorIndex].colorMin,
                                   m_parameters.colors[colorIndex].colorMax));
    }
}

void ParticleEmitter::setAngle(float baseAngle_rad, float spreadAngle_rad)
{
    m_parameters.baseAngle_rad  = baseAngle_rad;
    m_parameters.spreadAngle_rad = spreadAngle_rad;

    m_angleDist = std::uniform_real_distribution<float>(
        m_parameters.baseAngle_rad - m_parameters.spreadAngle_rad / 2,
        m_parameters.baseAngle_rad + m_parameters.spreadAngle_rad / 2);

    // for (uint32_t i = 0; i < m_numLiveParticles; ++i)
    // {
    //     float angle = m_angleDist(m_randGen);
    //     float speed = m_speedDist(m_randGen);

    //     m_particleAttributes[i].setVelocity(glm::vec3(
    //                 std::sin(angle) * speed, std::cos(angle) * speed, 0.0f));
    // }
}

glm::vec4 ParticleEmitter::_getRandomColorInRange(const glm::vec4& min,
                                                  const glm::vec4& max)
{
    float r = min.r + (m_gpRandDist(m_randGen) * (max.r - min.r));
    float g = min.g + (m_gpRandDist(m_randGen) * (max.g - min.g));
    float b = min.b + (m_gpRandDist(m_randGen) * (max.b - min.b));
    float a = min.a + (m_gpRandDist(m_randGen) * (max.a - min.a));

    return {r, g, b, a};
}

}  // namespace nimbus
