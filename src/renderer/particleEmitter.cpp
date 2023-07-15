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
                                 const glm::vec3&    centerPosition,
                                 uint32_t            count,
                                 float               particleLifetime,
                                 bool                persist,
                                 bool                is3d)
    : mp_shader(p_shader),
      mp_texture(p_texture),
      m_centerPosition(centerPosition),
      m_numLiveParticles(count),
      m_particleLifetime(particleLifetime),
      m_persist(persist),
      m_is3d(is3d)
{
    NM_CORE_ASSERT(!is3d, "3D spaces particles are not supported!");

    if (!is3d)
    {
        const std::vector<vertexData2d> vData = {
            // pos      // tex
            {glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},  // bottom left
            {glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},  // bottom right
            {glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},  // top right
            {glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 1.0f)}   // top left
        };

        std::vector<uint8_t> perVertexIdx = {
            0,
            1,
            2,  // first triangle
            2,
            3,
            0  // second triangle
        };

        mp_vao = makeRef<VertexArray>();

        ref<VertexBuffer> vertexVbo = makeRef<VertexBuffer>(
            &vData[0], vData.size() * sizeof(vertexData2d));

        vertexVbo->setFormat(ParticleEmitter::k_vertexVboFormat2d);

        mp_vao->addVertexBuffer(vertexVbo);

        mp_vao->setIndexBuffer(
            makeRef<IndexBuffer>(&perVertexIdx[0], perVertexIdx.size()));

        // todo, noodle through how to make us not need to copy this out each
        // time
        std::vector<Particle::instanceData> particleInstanceData;
        particleInstanceData.reserve(count);

        std::random_device               rd;
        std::mt19937                     gen(rd());
        std::uniform_real_distribution<float> dis(0, 1.0);
        std::uniform_real_distribution<float> disVelocity(0.0f, 2.0f * M_PI);


        for (uint32_t i = 0; i < count; i++)
        {
            float angle = disVelocity(gen);
            float speed = dis(gen);

            m_particles.emplace_back(
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(
                    std::sin(angle) * speed, std::cos(angle) * speed, 0.0f),
                glm::vec4(dis(gen), dis(gen), dis(gen), 1.0f),
                particleLifetime);

            particleInstanceData.emplace_back(m_particles[i].getInstanceData());
        }

        mp_instanceVbo = makeRef<VertexBuffer>(
            &particleInstanceData[0],
            particleInstanceData.size() * sizeof(Particle::instanceData),
            VertexBuffer::Type::STREAM_DRAW);

        mp_instanceVbo->setFormat(Particle::k_instanceVboFormat);

        mp_vao->addVertexBuffer(mp_instanceVbo);
    }
}

void ParticleEmitter::update(float deltaTime)
{
    NM_PROFILE_DETAIL();

    const uint32_t curLiveParticles = m_numLiveParticles;
    for (uint32_t i = 0; i < curLiveParticles; ++i)
    {
        m_particles[i].update(deltaTime);

        // dead particles are at the end of the vector
        if (m_particles[i].isDead())
        {
            std::swap(m_particles[i], m_particles.back());
            --m_numLiveParticles;
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

    // todo, noodle through how to make us not need to copy this out each time
    std::vector<Particle::instanceData> particleInstanceData;
    particleInstanceData.reserve(m_numLiveParticles);

    for (uint32_t i = 0; i < m_numLiveParticles; ++i)
    {
        particleInstanceData.emplace_back(m_particles[i].getInstanceData());
    }

    const std::string& uniformNm = mp_texture->getUniformNm(0);
    mp_shader->setInt(uniformNm, 0);
    mp_texture->bind(0);

    mp_instanceVbo->bind();
    mp_instanceVbo->setData(
        &particleInstanceData[0],
        particleInstanceData.size() * sizeof(Particle::instanceData));

    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::translate(model, m_centerPosition);
    model           = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));

    Renderer::submitInstanced(
        mp_shader, mp_vao, particleInstanceData.size(), model);

    Texture::s_unbind();
}

}  // namespace nimbus
