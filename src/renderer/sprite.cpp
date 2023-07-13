#include "renderer/sprite.hpp"

namespace nimbus
{

Sprite::Sprite(const std::string& textureFileNm,
               const std::string& vertShaderFileNm,
               const std::string& fragShaderFileNm,
               const glm::mat4&   projection)
{

    const float spriteVertices[] = { 
            // pos      // tex
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 
        
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f
        };

    const uint32_t places = 4;
    uint32_t numVertices  = sizeof(spriteVertices) / sizeof(float) / places;

    std::vector<Mesh::Vertex> spriteVerts;

    for (uint32_t i = 0; i < numVertices; i++)
    {
        const float* p_vertChunk = &spriteVertices[i * places];

        Mesh::Vertex vertex   = {};
        vertex.position = glm::vec3(p_vertChunk[0], p_vertChunk[1], 0.0);

        vertex.texCoords = glm::vec2(p_vertChunk[2], p_vertChunk[3]);

        spriteVerts.push_back(vertex);
    }

    ResourceManager& rm = ResourceManager::get();

    Texture* p_spriteTexture
        = rm.loadTexture(Texture::Type::DIFFUSE, textureFileNm);

    std::vector<Texture*> spriteTextures = {p_spriteTexture};

    mp_mesh = makeScope<Mesh>(spriteVerts, spriteTextures);

    Shader* p_shader = rm.loadShader(vertShaderFileNm, fragShaderFileNm);

    mp_mesh->setShader(p_shader);

    mp_mesh->getShader()->use();
    mp_mesh->getShader()->setMat4("projection", projection);
}

void Sprite::draw(const glm::vec2& pos,
                  const glm::vec2& size,
                  const glm::vec3& color,
                  const float      rotation)
{
    glm::mat4 model = glm::mat4(1.0f);
    // first translate (transformations are: scale happens first, then
    // rotation, and then final translation happens; reversed order)
    model = glm::translate(model, glm::vec3(pos, 0.0f));
    // move origin of rotation to center of quad
    model
        = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    // then rotate
    model = glm::rotate(
        model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    // move origin back
    model = glm::translate(model,
                           glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    // last scale
    model = glm::scale(model, glm::vec3(size, 1.0f));

    // only change the shader if this sprite uses a different shader
    // then the last sprite
    const Shader* p_shader = mp_mesh->getShader();
    if (p_shader != p_lastShader)
    {
        p_shader->use();
        p_lastShader = p_shader;
    }

    p_shader->setMat4("model", model);
    p_shader->setVec3("spriteColor", color);
    mp_mesh->draw();
}

}  // namespace nimbus