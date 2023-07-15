#pragma once
#include "common.hpp"
#include "glm.hpp"
#include "renderer/particle.hpp"
#include "platform/buffer.hpp"
#include "renderer/shader.hpp"
#include "renderer/texture.hpp"

#include <vector>

namespace nimbus
{
class ParticleEmitter
{
   public:
    // buffer format to describe the structure
    inline static const BufferFormat k_vertexVboFormat3d = {
        {k_shaderVec3, "vertexPosition"},
        {k_shaderVec2, "texCoords"},
    };

    inline static const BufferFormat k_vertexVboFormat2d = {
        {k_shaderVec2, "vertexPosition"},
        {k_shaderVec2, "texCoords"},
    };

    ParticleEmitter(const ref<Shader>&  p_shader,
                    const ref<Texture>& p_texture,
                    const glm::vec3&    centerPosition,
                    uint32_t            count,
                    float               particleLifetime,
                    bool                persist = false,
                    bool                is3d = false);

    ~ParticleEmitter() = default;

    void update(float deltaTime);

    void draw();

   private:
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

    std::vector<Particle> m_particles;
    ref<Shader>           mp_shader  = nullptr;
    ref<VertexArray>      mp_vao         = nullptr;
    ref<Texture>          mp_texture     = nullptr;
    ref<VertexBuffer>     mp_instanceVbo = nullptr;
    glm::vec3             m_centerPosition;
    uint32_t              m_numLiveParticles;
    float                 m_particleLifetime;
    bool                  m_persist;
    bool                  m_is3d;
};
}  // namespace nimbus
