#include "renderer/renderer2D.hpp"

#include "core.hpp"
#include "nmpch.hpp"
#include "platform/rendererApi.hpp"
#include "renderer/renderer.hpp"
#include "renderer/renderer2D.hpp"

namespace nimbus
{

const std::string k_particleVertexShader = R"(
    #version 460 core

    layout (location = 0) in vec2 aBasePos;
    layout (location = 1) in vec2 aTexCoords;

    layout (location = 2) in vec3 aParticlePosition;
    layout (location = 3) in vec4 aParticleColor;
    layout (location = 4) in float aParticleSize;

    out vec2 TexCoords;
    out vec4 Color;

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

    in vec2 TexCoords;
    in vec4 Color;

    out vec4 FragColor;

    uniform sampler2D particleTexture;

    void main()
    {
        // Sample the texture at the given coordinates and multiply it by the 
        // color
        FragColor = texture(particleTexture, TexCoords) * Color;
    }
)";


Renderer2D::TextData Renderer2D::s_textData;

void Renderer2D::init()
{
    ///////////////////////////
    // Text data init
    ///////////////////////////
    s_textData.vertices
        = std::vector<TextVertexInstance>(k_textVerticesInitCount);

    // clang-format off
    const std::vector<TextVertex> vData = 
    {
        // centered around (0, 0)
        // pos
        {glm::vec3(-0.5f, -0.5f, 0.0f)},  // bottom left
        {glm::vec3( 0.5f, -0.5f, 0.0f)},  // bottom right
        {glm::vec3( 0.5f,  0.5f, 0.0f)},  // top right
        {glm::vec3(-0.5f,  0.5f, 0.0f)}   // top left
    };

    std::vector<uint8_t> perVertexIdx = {
        0, 1, 2,  // first triangle
        2, 3, 0  // second triangle
    };
    // clang-format on

    s_textData.p_vao = makeRef<VertexArray>();

    ref<VertexBuffer> vertexVbo
        = makeRef<VertexBuffer>(&vData[0],
                                vData.size() * sizeof(TextVertex),
                                VertexBuffer::Type::STATIC_DRAW);

    vertexVbo->setFormat(k_TextVertexFormat);

    s_textData.p_vao->addVertexBuffer(vertexVbo);

    s_textData.p_vao->setIndexBuffer(
        makeRef<IndexBuffer>(&perVertexIdx[0], perVertexIdx.size()));

    // allocate buffer for instance data
    s_textData.p_vbo = makeRef<VertexBuffer>(
        &s_textData.vertices[0],
        s_textData.vertices.size() * sizeof(TextVertexInstance),
        VertexBuffer::Type::STREAM_DRAW);

    s_textData.p_vbo->setFormat(k_TextVertexInstanceFormat);

    s_textData.p_vao->addVertexBuffer(s_textData.p_vbo);

    s_textData.charCount = 0;
}

void Renderer2D::destroy()
{
}

void Renderer2D::setScene(Camera& camera)
{
    Renderer::setScene(camera);
}

void Renderer2D::drawText(const std::string& text, const Font& font)
{


}

}  // namespace nimbus