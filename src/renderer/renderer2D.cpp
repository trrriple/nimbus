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
    static std::once_flag initFlag;
    std::call_once(
        initFlag,
        []()
        {
        ///////////////////////////
        // General data init
        ///////////////////////////
        // clang-format off
        // bottom left
        s_genData.quadVertexPositions[0]= glm::vec4(-0.5f, -0.5f,  0.0f,  1.0f);
        s_genData.quadTextureCoords[0]   = glm::vec2( 0.0f,  0.0f);
        // bottom right
        s_genData.quadVertexPositions[1]= glm::vec4( 0.5f, -0.5f,  0.0f,  1.0f);
        s_genData.quadTextureCoords[1]   = glm::vec2( 1.0f,  0.0f);
        // top right
        s_genData.quadVertexPositions[2]= glm::vec4( 0.5f,  0.5f,  0.0f,  1.0f);
        s_genData.quadTextureCoords[2]   = glm::vec2( 1.0f,  1.0f);
        // top left
        s_genData.quadVertexPositions[3]= glm::vec4(-0.5f,  0.5f,  0.0f,  1.0f);
        s_genData.quadTextureCoords[3]   = glm::vec2( 0.0f,  1.0f);

        // clang-format on

        ///////////////////////////
        // Text data init
        ///////////////////////////
        s_textData.p_shader = ResourceManager::get().loadShader(
            "../resources/breakout/shaders/text.v.glsl",
            "../resources/breakout/shaders/text.f.glsl");

        _s_createTextBuffers();
    });
}

void Renderer2D::destroy()
{ 
}

void Renderer2D::begin(Camera& camera)
{   
    if(s_genData.inScene)
    {
        Log::coreError(
            "Renderer2D::begin called again without  Renderer2D::end!");
        return;
    }

    Renderer::setScene(camera);

    s_genData.inScene = true;
}

void Renderer2D::end()
{   
    if(!s_genData.inScene)
    {
        Log::coreError(
            "Renderer2D::end called before Renderer2D::begin!");
        return;
    }
    _s_submit();

    s_genData.inScene = false;
}

void Renderer2D::drawText(const std::string& text,
                          const Font&        font,
                          const glm::vec3&         pos)

{
    const auto& fontData     = font.getFontData();
    const auto& fontGeometry = fontData->fontGeometry;
    const auto& fontMetrics  = fontGeometry.getMetrics();

    s_textData.p_atlas = font.getAtlasTex();
    float atlasWidth   = s_textData.p_atlas->getWidth();
    float atlasHeight  = s_textData.p_atlas->getHeight();

    s_textData.unitRange
        = glm::vec2(fontData->pixelRange) / glm::vec2(atlasWidth, atlasHeight);

    float kerning = 0.0f;

    double incX        = 0.0;  // incremental x position
    double incY        = 0.0;  // incremental y positioncp
    double scale       = 1.0 / (fontMetrics.ascenderY - fontMetrics.descenderY);
    float  texelWidth  = 1.0f / atlasWidth;
    float  texelHeight = 1.0f / atlasHeight;

    for (uint32_t i = 0; i < text.size(); i++)
    {
        // verify we have room left for this character
        if(s_textData.vertexIdx + 4 > s_textData.vertices.size())
        {
            // We need to submit data to render.
            _s_submit();
            _s_createTextBuffers();
        }
        
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
        texRight  *= texelWidth;
        texTop    *= texelHeight;

        s_textData.vertices[s_textData.vertexIdx].position
            = glm::vec4(quadLeft, quadBottom, 0.0f, 1.0f);
        s_textData.vertices[s_textData.vertexIdx].fgColor
            = glm::vec4(1.0, 0.0, 0.0, 1.0f);  // TODO
        s_textData.vertices[s_textData.vertexIdx].bgColor
            = glm::vec4(0.0f);  // TODO
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

        s_textData.charCount++;

        if (i < text.size() - 1)
        {
            double advance       = glyph->getAdvance();
            char   nextCharacter = text[i + 1];
            fontGeometry.getAdvance(advance, character, nextCharacter);

            incX += scale * advance + kerning;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
// Private functions
////////////////////////////////////////////////////////////////////////////////
void Renderer2D::_s_submit()
{
    ///////////////////////////
    // Text Rendering
    ///////////////////////////
    if (s_textData.charCount > 0)
    {
        s_textData.p_vbo->setData(&s_textData.vertices[0],
                                  sizeof(TextVertex) * s_textData.vertexIdx);
        s_textData.p_atlas->bind(0);
        s_textData.p_shader->use();
        s_textData.p_shader->setInt("u_fontAtlas", 0);
        s_textData.p_shader->setVec2("u_unitRange", s_textData.unitRange);

        Renderer::submit(                           // 6 vertex per char
            s_textData.p_shader, s_textData.p_vao, s_textData.charCount * 6);
    
        s_textData.vertexIdx = 0;
        s_textData.charCount = 0;
    }
}

void Renderer2D::_s_createTextBuffers()
{
    ///////////////////////////
    // Determine size
    ///////////////////////////
    uint32_t newSize;
    if (s_textData.p_vao)
    {
        // this is a resize
        s_textData.p_vao->unbind();

        newSize = s_textData.vertices.size() + k_textVerticesGrowCount;

        if (newSize >= k_textVerticesMaxCount)
        {
            Log::coreWarn("Text vertex buffer size limit (%i) reached.",
                          k_textVerticesMaxCount);
            return;
        }

    }
    else
    {
        newSize = k_textVerticesInitCount;
    }

    ///////////////////////////
    // (Re)make VAO
    ///////////////////////////
    s_textData.p_vao = makeRef<VertexArray>();

    ///////////////////////////
    // (Re)make VBO
    ///////////////////////////
    s_textData.vertices
        = std::vector<TextVertex>(newSize);

    s_textData.p_vbo
        = makeRef<VertexBuffer>(&s_textData.vertices[0],
                                s_textData.vertices.size() * sizeof(TextVertex),
                                VertexBuffer::Type::DYNAMIC_DRAW);

    s_textData.p_vbo->setFormat(k_TextVertexFormat);
    s_textData.p_vao->addVertexBuffer(s_textData.p_vbo);

    ///////////////////////////
    // (Re)make IBO
    ///////////////////////////
    std::vector<uint16_t> quadVertexIndices;
    quadVertexIndices.reserve(6 * newSize);

    uint16_t offset = 0;
    for (uint32_t i = 0; i < newSize; i++)
    {   
        quadVertexIndices.push_back(0 + offset);
        quadVertexIndices.push_back(1 + offset);
        quadVertexIndices.push_back(2 + offset);
        quadVertexIndices.push_back(2 + offset);
        quadVertexIndices.push_back(3 + offset);
        quadVertexIndices.push_back(0 + offset);
        offset += 4;
    }

    s_textData.p_vao->setIndexBuffer(
        makeRef<IndexBuffer>(&quadVertexIndices[0], quadVertexIndices.size()));


    s_textData.charCount = 0;
    s_textData.vertexIdx = 0;

    Log::coreInfo("Text vertex buffer sized to %i", newSize);

}

}  // namespace nimbus