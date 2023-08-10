#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/renderer/renderer2D.hpp"

#include "nimbus/renderer/graphicsApi.hpp"
#include "nimbus/renderer/fontData.hpp"
#include "nimbus/renderer/renderer.hpp"
#include "nimbus/renderer/renderer2D.hpp"
#include "nimbus/core/resourceManager.hpp"
#include "nimbus/core/application.hpp"

namespace nimbus
{

bool                  Renderer2D::s_inScene = false;
Renderer2D::QuadData* Renderer2D::s_quadData;
Renderer2D::TextData* Renderer2D::s_textData;
Renderer2D::Stats     Renderer2D::s_stats;

void Renderer2D::s_init()
{
    static std::once_flag initFlag;
    std::call_once(
        initFlag,
        []()
        {
            s_quadData = new QuadData;
            s_textData = new TextData;

            ///////////////////////////
            // General data init
            ///////////////////////////
            s_quadData->vertices.push_back(
                {glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)});
            s_quadData->vertices.push_back(
                {glm::vec4(0.5f, -0.5f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)});
            s_quadData->vertices.push_back(
                {glm::vec4(0.5f, 0.5f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)});
            s_quadData->vertices.push_back(
                {glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)});

            ///////////////////////////
            // Quad data init
            ///////////////////////////
            s_quadData->p_shader
                = Application::s_get().getResourceManager().loadShader(
                    "../resources/shaders/quad.v.glsl",
                    "../resources/shaders/quad.f.glsl");

            s_quadData->textures.reserve(Texture::s_getMaxTextures());

            Texture::Spec texSpec;
            texSpec.width  = 1;
            texSpec.height = 1;
            ref<Texture> whiteTexture
                = Texture::s_create(Texture::Type::DIFFUSE, texSpec);

            // being a 1x1 texture, it's only 4 bytes of data
            uint32_t whiteData = 0xFFFFFFFF;
            whiteTexture->setData(&whiteData, sizeof(whiteData));
            s_quadData->textures.push_back(whiteTexture);

            _s_createQuadBuffers();

            ///////////////////////////
            // Text data init
            ///////////////////////////
            s_textData->p_shader
                = Application::s_get().getResourceManager().loadShader(
                    "../resources/shaders/text.v.glsl",
                    "../resources/shaders/text.f.glsl");

            s_textData->atlases.reserve(Texture::s_getMaxTextures());

            _s_createTextBuffers();
        });
}

void Renderer2D::s_destroy()
{
    delete s_quadData;
    delete s_textData;
}

void Renderer2D::s_begin(const glm::mat4& vpMatrix)
{
    NM_PROFILE_TRACE();

    if (s_inScene)
    {
        Log::coreError(
            "Renderer2D::begin called again without  Renderer2D::end!");
        return;
    }

    Renderer::s_setScene(vpMatrix);
    s_inScene = true;
}

void Renderer2D::s_end()
{
    NM_PROFILE_TRACE();

    if (!s_inScene)
    {
        Log::coreError("Renderer2D::end called before Renderer2D::begin!");
        return;
    }
    _s_submit();

    s_inScene = false;

    // resize buffers if required
    if (s_quadData->needsResize)
    {
        _s_createQuadBuffers();
    }

    if (s_textData->needsResize)
    {
        _s_createTextBuffers();
    }
}

void Renderer2D::s_drawQuad(const glm::mat4&    transform,
                            const ref<Texture>& p_texture,
                            const glm::vec4&    color,
                            float               texTilingFactor,
                            int                 entityId)
{
    // first make sure we can fit this quad:
    //  verify we have room left for this character
    if (s_quadData->quadCount + 1 > s_quadData->instVertices.size())
    {
        // We're full, We need to submit data to render.
        _s_submit();
        // this will resize the buffers if possible on begin()
        s_quadData->needsResize = true;
    }

    int texIdx = 0;
    if (p_texture != nullptr)
    {
        for (size_t i = 1; i < s_quadData->textures.size(); i++)
        {
            if (*(s_quadData->textures[i]) == *p_texture)
            {
                // this fellow is the same, so reuse it
                texIdx = static_cast<int>(i);
                break;
            }
        }
        // we didn't find it, so it's a new texture
        if (texIdx == 0)
        {
            if (s_quadData->textures.size() == s_quadData->textures.capacity())
            {
                // we've used all texture slots, send it;
                _s_submit();
            }

            // grab the location and store it
            texIdx = s_quadData->textures.size();
            s_quadData->textures.push_back(p_texture);
        }
    }
    else
    {
        texIdx = 0;  // white texture
    }

    s_quadData->instVertices[s_quadData->quadCount].transform = transform;
    s_quadData->instVertices[s_quadData->quadCount].color     = color;
    s_quadData->instVertices[s_quadData->quadCount].texIndex  = texIdx;
    s_quadData->instVertices[s_quadData->quadCount].texTilingFactor
        = texTilingFactor;
    s_quadData->instVertices[s_quadData->quadCount].entityId = entityId;

    s_quadData->quadCount++;
}

void Renderer2D::s_drawQuad(const glm::mat4& transform,
                            const glm::vec4& color,
                            int              entityId)
{
    NM_PROFILE_TRACE();
    s_drawQuad(transform, nullptr, color, 1.0f, entityId);
}

void Renderer2D::s_drawText(const std::string&  text,
                            const Font::Format& fontFormat,
                            const glm::vec3&    position,
                            const glm::vec2&    size,
                            int                 entityId)
{
    NM_PROFILE_TRACE();
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
    transform           = glm::scale(transform, {size.x, size.y, 1.0f});
    s_drawText(text, fontFormat, transform, entityId);
}

void Renderer2D::s_drawText(const std::string&  text,
                            const Font::Format& fontFormat,
                            const glm::mat4&    transform,
                            int                 entityId)

{
    NM_PROFILE();

    // can't render without a font
    if (fontFormat.p_font == nullptr)
    {
        return;
    }

    // fonts are loaded in seperate threads, check if it's loaded yet
    // before trying to draw with it
    if (!fontFormat.p_font->isLoaded())
    {
        return;
    }

    const auto& fontData     = fontFormat.p_font->getFontData();
    const auto& fontGeometry = fontData->fontGeometry;
    const auto& fontMetrics  = fontGeometry.getMetrics();
    const float spaceAdvance = fontGeometry.getGlyph(' ')->getAdvance();

    auto p_atlas = fontFormat.p_font->getAtlasTex();

    // calc data we need
    float     atlasWidth  = p_atlas->getWidth();
    float     atlasHeight = p_atlas->getHeight();
    glm::vec2 unitRange
        = glm::vec2(fontData->pixelRange) / glm::vec2(atlasWidth, atlasHeight);

    double incX        = 0.0;  // incremental x position
    double incY        = 0.0;  // incremental y positioncp
    double scale       = 1.0 / (fontMetrics.ascenderY - fontMetrics.descenderY);
    float  texelWidth  = 1.0f / atlasWidth;
    float  texelHeight = 1.0f / atlasHeight;

    int texIdx = -1;
    for (size_t i = 0; i < s_textData->atlases.size(); i++)
    {
        if (*(s_textData->atlases[i]) == *p_atlas)
        {
            // this fellow is the same, so reuse it
            texIdx = static_cast<int>(i);
            break;
        }
    }
    // we didn't find it, so it's a new texture
    if (texIdx == -1)
    {
        if (s_textData->atlases.size() == s_textData->atlases.capacity())
        {
            // we've used all texture slots, send it;
            _s_submit();
        }

        // grab the location and store it
        texIdx = s_textData->atlases.size();
        s_textData->atlases.push_back(p_atlas);
    }

    for (uint32_t i = 0; i < text.size(); i++)
    {
        char character = text[i];

        // some characters we don't need to render, we just need to
        // adjust where the character after that will be (like spaces)
        if (character == '\x20')
        {
            // if this isn't the last character
            if (i < text.size() - 1)
            {
                double advance;
                if (!fontGeometry.getAdvance(advance, character, text[i + 1]))
                {
                    // we couldn't get a specific advance for this character
                    // combination, so just use the one for this character
                    advance = spaceAdvance;
                }

                // kerning doesn't apply to spaces
                incX += scale * static_cast<float>(advance);
                continue;
            }
        }
        else if (character == '\n')
        {
            // newlines just reset us back to zero x, and move y down.
            incX = 0.0f;
            incY -= scale * fontMetrics.lineHeight + fontFormat.leading;
            continue;
        }
        else if (character == '\t')
        {
            // a tab is 4 space characters, specific character pair advance
            // doesn't matter for tabs either
            incX += scale * (spaceAdvance * 4.0);
            continue;
        }
        else if (character == '\r')
        {
            // we don't need to do anything with this
            continue;
        }

        // verify we have room left for this character
        if (s_textData->vertexIdx + 4 > s_textData->vertices.size())
        {
            // We need to submit data to render.
            _s_submit();
            // this will resize the buffers if possible on begin()
            s_textData->needsResize = true;
        }

        // we need to find the character in the atlas
        auto glyph = fontGeometry.getGlyph(character);

        if (!glyph)
        {
            // we didn't find it so use a ? if it's there
            glyph = fontGeometry.getGlyph('?');
            Log::coreWarn("Glyph %c not found in font %s",
                          character,
                          fontFormat.p_font->getPath().c_str());
        }
        // whats the deal with this font?
        NM_CORE_ASSERT_STATIC(
            glyph,
            "Glyph %c not found in font %s (nor was ? for that matter)",
            character,
            fontFormat.p_font->getPath().c_str());

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

        texLeft *= texelWidth;
        texBottom *= texelHeight;
        texRight *= texelWidth;
        texTop *= texelHeight;

        s_textData->vertices[s_textData->vertexIdx].position
            = transform * glm::vec4(quadLeft, quadBottom, 0.0f, 1.0f);
        s_textData->vertices[s_textData->vertexIdx].texCoord
            = glm::vec2(texLeft, texBottom);
        s_textData->vertices[s_textData->vertexIdx].fgColor
            = fontFormat.fgColor;
        s_textData->vertices[s_textData->vertexIdx].bgColor
            = fontFormat.bgColor;
        s_textData->vertices[s_textData->vertexIdx].unitRange = unitRange;
        s_textData->vertices[s_textData->vertexIdx].texIndex  = texIdx;
        s_textData->vertices[s_textData->vertexIdx].entityId  = entityId;

        s_textData->vertexIdx++;

        s_textData->vertices[s_textData->vertexIdx].position
            = transform * glm::vec4(quadLeft, quadTop, 0.0f, 1.0f);
        s_textData->vertices[s_textData->vertexIdx].texCoord
            = glm::vec2(texLeft, texTop);
        s_textData->vertices[s_textData->vertexIdx].fgColor
            = fontFormat.fgColor;
        s_textData->vertices[s_textData->vertexIdx].bgColor
            = fontFormat.bgColor;
        s_textData->vertices[s_textData->vertexIdx].unitRange = unitRange;
        s_textData->vertices[s_textData->vertexIdx].texIndex  = texIdx;
        s_textData->vertices[s_textData->vertexIdx].entityId  = entityId;

        s_textData->vertexIdx++;

        s_textData->vertices[s_textData->vertexIdx].position
            = transform * glm::vec4(quadRight, quadTop, 0.0f, 1.0f);
        s_textData->vertices[s_textData->vertexIdx].texCoord
            = glm::vec2(texRight, texTop);
        s_textData->vertices[s_textData->vertexIdx].fgColor
            = fontFormat.fgColor;
        s_textData->vertices[s_textData->vertexIdx].bgColor
            = fontFormat.bgColor;
        s_textData->vertices[s_textData->vertexIdx].unitRange = unitRange;
        s_textData->vertices[s_textData->vertexIdx].texIndex  = texIdx;
        s_textData->vertices[s_textData->vertexIdx].entityId  = entityId;

        s_textData->vertexIdx++;

        s_textData->vertices[s_textData->vertexIdx].position
            = transform * glm::vec4(quadRight, quadBottom, 0.0f, 1.0f);
        s_textData->vertices[s_textData->vertexIdx].texCoord
            = glm::vec2(texRight, texBottom);
        s_textData->vertices[s_textData->vertexIdx].fgColor
            = fontFormat.fgColor;
        s_textData->vertices[s_textData->vertexIdx].bgColor
            = fontFormat.bgColor;
        s_textData->vertices[s_textData->vertexIdx].unitRange = unitRange;
        s_textData->vertices[s_textData->vertexIdx].texIndex  = texIdx;
        s_textData->vertices[s_textData->vertexIdx].entityId  = entityId;

        s_textData->vertexIdx++;

        s_textData->charCount++;

        if (i < text.size() - 1)
        {
            double advance;
            if (!fontGeometry.getAdvance(advance, character, text[i + 1]))
            {
                // we couldn't get a specific advance for this character
                // combination, so just use the one for this character
                advance = glyph->getAdvance();
            }

            incX += scale * static_cast<float>(advance) + fontFormat.kerning;
        }
    }
}

void Renderer2D::s_resetStats()
{
    s_stats = Stats();
}

////////////////////////////////////////////////////////////////////////////////
// Private functions
////////////////////////////////////////////////////////////////////////////////
void Renderer2D::_s_submit()
{
    NM_PROFILE();

    ///////////////////////////
    // Quad Rendering
    ///////////////////////////
    if (s_quadData->quadCount > 0)
    {
        s_quadData->p_vbo->setData(
            &s_quadData->instVertices[0],
            sizeof(QuadInstVertex) * s_quadData->quadCount);

        for (size_t i = 0; i < s_quadData->textures.size(); i++)
        {
            s_quadData->textures[i]->bind(i);
        }

        Renderer::s_renderInstanced(
            s_quadData->p_shader, s_quadData->p_vao, s_quadData->quadCount);

        // collect stats
        s_stats.drawCalls++;
        s_stats.quads += s_quadData->quadCount;
        s_stats.quadVertices += s_quadData->quadCount * 4;
        s_stats.totalVertices += s_quadData->quadCount * 4;
        s_stats.quadVertsAvail
            = (s_quadData->instVertices.size() * 4) - s_stats.quadVertices;

        // reset
        s_quadData->quadCount = 0;

        // remove all but the first texture (the white one)
        s_quadData->textures.erase(s_quadData->textures.begin() + 1,
                                   s_quadData->textures.end());
    }

    ///////////////////////////
    // Text Rendering
    ///////////////////////////
    if (s_textData->charCount > 0)
    {
        s_textData->p_vbo->setData(&s_textData->vertices[0],
                                   sizeof(TextVertex) * s_textData->vertexIdx);

        for (size_t i = 0; i < s_textData->atlases.size(); i++)
        {
            s_textData->atlases[i]->bind(i);
        }

        Renderer::s_render(  // 6 vertex per quad
            s_textData->p_shader,
            s_textData->p_vao,
            s_textData->charCount * 6,
            true);

        // collect stats
        s_stats.drawCalls++;
        s_stats.characters += s_textData->charCount;
        s_stats.textVertices += s_textData->charCount * 4;
        s_stats.totalVertices += s_textData->charCount * 4;
        s_stats.textVertsAvail
            = s_textData->vertices.size() - s_stats.textVertices;

        // reset
        s_textData->vertexIdx = 0;
        s_textData->charCount = 0;

        s_textData->atlases.clear();
    }
}

template <typename IndexType>
static void s_generateIndicesAndSetBuffer(uint32_t          quads,
                                          ref<VertexArray>& p_vao)
{
    std::vector<IndexType> quadVertexIndices;
    quadVertexIndices.reserve(6 * quads);

    IndexType offset = 0;
    for (uint32_t i = 0; i < quads; i++)
    {
        quadVertexIndices.push_back(0 + offset);
        quadVertexIndices.push_back(1 + offset);
        quadVertexIndices.push_back(2 + offset);
        quadVertexIndices.push_back(2 + offset);
        quadVertexIndices.push_back(3 + offset);
        quadVertexIndices.push_back(0 + offset);
        offset += 4;
    }

    p_vao->setIndexBuffer(
        IndexBuffer::s_create(&quadVertexIndices[0], quadVertexIndices.size()));
}

void Renderer2D::_s_createQuadBuffers()
{
    NM_PROFILE_DETAIL();

    ///////////////////////////
    // Determine size
    ///////////////////////////
    uint32_t newSize;
    if (s_quadData->p_vao)
    {
        // this is a resize
        newSize = s_quadData->instVertices.size() + k_quadGrowCount;

        if (newSize >= k_quadMaxCount)
        {
            Log::coreWarn("Quad buffer size limit (%i) reached.",
                          k_quadMaxCount);
            return;
        }
    }
    else
    {
        newSize = k_quadInitCount;
    }

    ///////////////////////////
    // (Re)make VAO
    ///////////////////////////
    s_quadData->p_vao = VertexArray::s_create();

    ///////////////////////////
    // (Re)make Shared VBO
    ///////////////////////////
    // shared, doesn't change after creation so no need to save handle to it
    auto sharedVbo = VertexBuffer::s_create(
        &s_quadData->vertices[0],
        s_quadData->vertices.size() * sizeof(QuadVertex),
        VertexBuffer::Type::STATIC_DRAW);

    sharedVbo->setFormat(k_quadVertexFormat);
    s_quadData->p_vao->addVertexBuffer(sharedVbo);

    ///////////////////////////
    // (Re)make IBO for shared
    ///////////////////////////
    s_generateIndicesAndSetBuffer<uint8_t>(1, s_quadData->p_vao);

    ///////////////////////////
    // (Re)make Instance VBO
    ///////////////////////////
    s_quadData->instVertices = std::vector<QuadInstVertex>(newSize);

    s_quadData->p_vbo = VertexBuffer::s_create(
        &s_quadData->instVertices[0],
        s_quadData->instVertices.size() * sizeof(QuadInstVertex),
        VertexBuffer::Type::STREAM_DRAW);

    s_quadData->p_vbo->setFormat(k_quadInstVertexFormat);
    s_quadData->p_vao->addVertexBuffer(s_quadData->p_vbo);

    s_quadData->quadCount   = 0;
    s_quadData->needsResize = false;

    Log::coreInfo("Quad buffer sized to %i", newSize);
}

void Renderer2D::_s_createTextBuffers()
{
    NM_PROFILE_DETAIL();

    ///////////////////////////
    // Determine size
    ///////////////////////////
    uint32_t newSize;
    if (s_textData->p_vao)
    {
        // this is a resize
        s_textData->p_vao->unbind();

        newSize = s_textData->vertices.size() + k_textVerticesGrowCount;

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
    s_textData->p_vao = VertexArray::s_create();

    ///////////////////////////
    // (Re)make VBO
    ///////////////////////////
    s_textData->vertices = std::vector<TextVertex>(newSize);

    s_textData->p_vbo = VertexBuffer::s_create(
        &s_textData->vertices[0],
        s_textData->vertices.size() * sizeof(TextVertex),
        VertexBuffer::Type::DYNAMIC_DRAW);

    s_textData->p_vbo->setFormat(k_TextVertexFormat);
    s_textData->p_vao->addVertexBuffer(s_textData->p_vbo);

    ///////////////////////////
    // (Re)make IBO
    ///////////////////////////
    uint32_t maxIndexVal = 6 * newSize - 1;

    if (maxIndexVal < 65536)
    {
        s_generateIndicesAndSetBuffer<uint16_t>(newSize / 4, s_textData->p_vao);
    }
    else
    {
        s_generateIndicesAndSetBuffer<uint32_t>(newSize / 4, s_textData->p_vao);
    }

    s_textData->charCount   = 0;
    s_textData->vertexIdx   = 0;
    s_textData->needsResize = false;

    Log::coreInfo("Text buffer sized to %i", newSize / 4);
}

}  // namespace nimbus