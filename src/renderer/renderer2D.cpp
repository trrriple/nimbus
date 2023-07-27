#include "nmpch.hpp"
#include "core.hpp"

#include "renderer/renderer2D.hpp"

#include "platform/rendererApi.hpp"
#include "renderer/renderer.hpp"
#include "renderer/renderer2D.hpp"
#include "renderer/fontData.hpp"
#include "resourceManager.hpp"

namespace nimbus
{

Renderer2D::TextData    Renderer2D::s_textData;
Renderer2D::GeneralData Renderer2D::s_genData;

void Renderer2D::init()
{
    ///////////////////////////
    // General data init
    ///////////////////////////
    // clang-format off
    // bottom left
    s_genData.quadVertexPositions[0] = glm::vec4(-0.5f, -0.5f,  0.0f,  1.0f);
    s_genData.quadTextureCoords[0]   = glm::vec2( 0.0f,  0.0f);
    // bottom right
    s_genData.quadVertexPositions[1] = glm::vec4( 0.5f, -0.5f,  0.0f,  1.0f);
    s_genData.quadTextureCoords[1]   = glm::vec2( 1.0f,  0.0f);
    // top right
    s_genData.quadVertexPositions[2] = glm::vec4( 0.5f,  0.5f,  0.0f,  1.0f);
    s_genData.quadTextureCoords[2]   = glm::vec2( 1.0f,  1.0f);
    // top left
    s_genData.quadVertexPositions[3] = glm::vec4(-0.5f,  0.5f,  0.0f,  1.0f);
    s_genData.quadTextureCoords[3]   = glm::vec2( 0.0f,  1.0f);

    std::vector<uint8_t> quadVertexIndices = {
        0, 1, 2,  // first triangle
        2, 3, 0  // second triangle
    };
    // clang-format on

    ///////////////////////////
    // Text data init
    ///////////////////////////
    s_textData.p_shader = ResourceManager::get().loadShader(
        "../resources/breakout/shaders/text.v.glsl",
        "../resources/breakout/shaders/text.f.glsl");

    s_textData.vertices = std::vector<TextVertex>(k_textVerticesInitCount);

    s_textData.p_vao = makeRef<VertexArray>();

    s_textData.p_vbo
        = makeRef<VertexBuffer>(&s_textData.vertices[0],
                                s_textData.vertices.size() * sizeof(TextVertex),
                                VertexBuffer::Type::DYNAMIC_DRAW);

    s_textData.p_vbo->setFormat(k_TextVertexFormat);

    s_textData.p_vao->addVertexBuffer(s_textData.p_vbo);

    s_textData.p_vao->setIndexBuffer(
        makeRef<IndexBuffer>(&quadVertexIndices[0], quadVertexIndices.size()));

    s_textData.charCount = 0;
}

void Renderer2D::destroy()
{ 
}

void Renderer2D::setScene(Camera& camera)
{
    Renderer::setScene(camera);
}

void Renderer2D::drawText(const std::string& text,
                          const Font&        font,
                          const glm::vec3&         pos)

{
    const auto& fontData     = font.getFontData();
    const auto& fontGeometry = fontData->fontGeometry;
    const auto& fontMetrics  = fontGeometry.getMetrics();

    s_textData.p_atlas = font.getAtlasTex();
    float atlasWidth     = s_textData.p_atlas->getWidth();
    float atlasHeight    = s_textData.p_atlas->getHeight();

    // TODO: remove this
    s_textData.vertexIdx = 0;
    

    float kerning = 0.0f;

    double incX        = 0.0;  // incremental x position
    double incY        = 0.0;  // incremental y positioncp
    double scale       = 1.0 / (fontMetrics.ascenderY - fontMetrics.descenderY);
    float  texelWidth  = 1.0f / atlasWidth;
    float  texelHeight = 1.0f / atlasHeight;

    for (uint32_t i = 0; i < text.size(); i++)
    {
        char character = text[i];

        // we need to find the character in the atlas
        auto glyph = fontGeometry.getGlyph(character);

        if (!glyph)
        {
            // we didn't find it so use a ? if it's there
            glyph = fontGeometry.getGlyph('?');
            Log::coreWarn("Glyph %c not found in font %s\n",
                          font.getPath().c_str());
        }
        // whats the deal with this font?
        NM_CORE_ASSERT_STATIC(
            glyph,
            "Glyph %c not found in font %s (nor was ? for that matter)\n",
            character,
            font.getPath().c_str());
    
    
        // figure out the bounds of the quad needed for glyph
        double quadLeft;
        double quadBottom;
        double quadRight;
        double quadTop;
        glyph->getQuadPlaneBounds(quadLeft, quadBottom, quadRight, quadTop);
        quadLeft   = (quadLeft * scale) + incX;
        quadBottom = (quadBottom * scale) + incY;
        quadRight  = (quadRight * scale) + incX;
        quadTop    = (quadTop * scale) + incY;

        // figure out the location in the atlas
        double texLeft;
        double texBottom;
        double texRight;
        double texTop;
        glyph->getQuadAtlasBounds(texLeft, texBottom, texRight, texTop);
        glm::vec2 texCoordMin((float)texLeft, (float)texBottom);
        glm::vec2 texCoordMax((float)texRight, (float)texTop);
        
        texLeft   *= texelWidth;
        texBottom *= texelHeight;
        texRight *= texelWidth;
        texTop *= texelHeight;

        s_textData.vertices[s_textData.vertexIdx].position
            = glm::vec4(quadLeft, quadBottom, 0.0f, 1.0f);
        s_textData.vertices[s_textData.vertexIdx].fgColor
            = glm::vec4(1.0, 0.0, 0.0, 1.0f);  // todo
        s_textData.vertices[s_textData.vertexIdx].bgColor
            = glm::vec4(0.0f);  // todo
        s_textData.vertices[s_textData.vertexIdx].texCoord
            = glm::vec2(texLeft, texBottom);
        s_textData.vertexIdx++;

        s_textData.vertices[s_textData.vertexIdx].position
            = glm::vec4(quadLeft, quadTop, 0.0f, 1.0f);
        s_textData.vertices[s_textData.vertexIdx].fgColor
            = glm::vec4(1.0, 0.0, 0.0, 1.0f);  // todo
        s_textData.vertices[s_textData.vertexIdx].bgColor
            = glm::vec4(0.0f);  // todo
        s_textData.vertices[s_textData.vertexIdx].texCoord
            = glm::vec2(texLeft, texTop);
        s_textData.vertexIdx++;

        s_textData.vertices[s_textData.vertexIdx].position
            = glm::vec4(quadRight, quadTop, 0.0f, 1.0f);
        s_textData.vertices[s_textData.vertexIdx].fgColor
            = glm::vec4(1.0, 0.0, 0.0, 1.0f);  // todo
        s_textData.vertices[s_textData.vertexIdx].bgColor
            = glm::vec4(0.0f);  // todo
        s_textData.vertices[s_textData.vertexIdx].texCoord
            = glm::vec2(texRight, texTop);
        s_textData.vertexIdx++;

        s_textData.vertices[s_textData.vertexIdx].position
            = glm::vec4(quadRight, quadBottom, 0.0f, 1.0f);
        s_textData.vertices[s_textData.vertexIdx].fgColor
            = glm::vec4(1.0, 0.0, 0.0, 1.0f);  // todo
        s_textData.vertices[s_textData.vertexIdx].bgColor
            = glm::vec4(0.0f);  // todo
        s_textData.vertices[s_textData.vertexIdx].texCoord
            = glm::vec2(texRight, texBottom);
        s_textData.vertexIdx++;
    }

    // temporary
    s_textData.p_vbo->setData(&s_textData.vertices[0], sizeof(TextVertex) * 4);
    s_textData.p_atlas->bind(0);
    s_textData.p_shader->use();
    s_textData.p_shader->setInt("u_fontAtlas", 0);
    s_textData.p_shader->setVec2(
        "u_unitRange",
        glm::vec2(fontData->pixelRange) / glm::vec2(atlasWidth, atlasHeight));

    Renderer::submit(s_textData.p_shader, s_textData.p_vao, 6);
}

}  // namespace nimbus