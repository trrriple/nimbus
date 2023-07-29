#pragma once

#include "camera.hpp"
#include "common.hpp"
#include "glm.hpp"
#include "platform/buffer.hpp"
#include "renderer/font.hpp"
#include "renderer/shader.hpp"

namespace nimbus
{
class Renderer2D
{
   public:
    struct TextFormat
    {
        ref<Font> p_font      = nullptr;
        glm::vec4 fgColor     = glm::vec4(1.0f);
        glm::vec4 bgColor     = glm::vec4(0.0f);
        float     kerning     = 0.0f;
        float     lineSpacing = 0.0f;
    };

    static void s_init();
    static void s_destroy();

    static void s_begin(Camera& camera);

    static void s_end();

    static void s_drawQuad(const glm::mat4&    transform,
                           const ref<Texture>& p_texture,
                           const glm::vec4&    color,
                           float               texTilingFactor = 1.0f);

    static void s_drawQuad(const glm::mat4& transform, const glm::vec4& color);

    static void s_drawText(const std::string& text,
                           const TextFormat&  textFormat,
                           const glm::vec3&   position,
                           const glm::vec2&   size);

    static void s_drawText(const std::string& text,
                           const TextFormat&  textFormat,
                           const glm::mat4&   transform);

   private:
    ///////////////////////////
    // Generic Data
    ///////////////////////////
    struct GeneralData
    {
        bool      inScene = false;
        glm::vec4 quadVertexPositions[4];
        glm::vec2 quadTextureCoords[4];
    };
   
    static GeneralData s_genData;


    ///////////////////////////
    //  Quad layout and data
    ///////////////////////////
    inline static const uint32_t k_quadVerticesInitCount = 5000;
    inline static const uint32_t k_quadVerticesGrowCount = 5000;
    inline static const uint32_t k_quadVerticesMaxCount  = 50000;

    inline static const BufferFormat k_quadVertexFormat = {
        {k_shaderVec4,  "position"},
        {k_shaderVec2,  "texCoords"},
        {k_shaderVec4,  "color"},
        {k_shaderFloat, "texIndex"},
        {k_shaderFloat, "texTilingFactor"},
    };

    struct QuadVertex
    {
        glm::vec4 position;
        glm::vec2 texCoord;
        glm::vec4 color;
        float     texIndex;
        float     texTilingFactor;
    };

    struct QuadData
    {
        std::vector<QuadVertex>   vertices;
        uint32_t                  vertexIdx = 0;
        ref<VertexBuffer>         p_vbo     = nullptr;
        ref<VertexArray>          p_vao     = nullptr;
        ref<Shader>               p_shader  = nullptr;
        uint32_t                  quadCount = 0;
        std::vector<ref<Texture>> textures;
        bool                      needsResize = false;
    };

    static QuadData s_quadData;

    ///////////////////////////
    //  Text layout and data
    ///////////////////////////
    inline static const uint32_t k_textVerticesInitCount = 500;
    inline static const uint32_t k_textVerticesGrowCount = 500;
    inline static const uint32_t k_textVerticesMaxCount  = 10000;

    inline static const BufferFormat k_TextVertexFormat = {
        {k_shaderVec4,  "position"},
        {k_shaderVec2,  "texCoords"},
        {k_shaderVec4,  "fgColor"},
        {k_shaderVec4,  "bgColor"},
        {k_shaderVec2,  "unitRange"},
        {k_shaderFloat, "texIndex"},
    };
    
    struct TextVertex
    {
        glm::vec4 position;
        glm::vec2 texCoord;
        glm::vec4 fgColor;
        glm::vec4 bgColor;
        glm::vec2 unitRange;
        float     texIndex;
    };

    struct TextData
    {
        std::vector<TextVertex>   vertices;
        uint32_t                  vertexIdx = 0;
        ref<VertexBuffer>         p_vbo     = nullptr;
        ref<VertexArray>          p_vao     = nullptr;
        ref<Shader>               p_shader  = nullptr;
        uint32_t                  charCount = 0;
        std::vector<ref<Texture>> atlases;
        bool                      needsResize = false;
    };

    static TextData s_textData;

    ///////////////////////////
    // Private functions
    ///////////////////////////
    static void _s_submit();
    static void _s_createTextBuffers();
    static void _s_createQuadBuffers();

};
}  // namespace nimbus