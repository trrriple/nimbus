#pragma once

#include "nimbus/core/common.hpp"
#include "nimbus/scene/camera.hpp"
#include "nimbus/renderer/buffer.hpp"
#include "nimbus/renderer/font.hpp"
#include "nimbus/renderer/shader.hpp"
#include "nimbus/renderer/graphicsApi.hpp"

#include "glm.hpp"

namespace nimbus
{
class Renderer2D : public refCounted
{
   public:
    struct Stats
    {
        uint32_t drawCalls     = 0;
        uint32_t quads         = 0;
        uint32_t characters    = 0;
        uint32_t quadVertices  = 0;
        uint32_t textVertices  = 0;
        uint32_t totalVertices = 0;

        uint32_t quadVertsAvail = 0;
        uint32_t textVertsAvail = 0;
    };

    static void s_init();

    static void s_destroy();

    static void s_begin(const glm::mat4& vpMatrix);

    static void s_end();

    static void s_drawQuad(const glm::mat4&    transform,
                           const ref<Texture>& p_texture,
                           const glm::vec4&    color,
                           float               texTilingFactor = 1.0f,
                           uint32_t            entityId        = 0);

    static void s_drawQuad(const glm::mat4& transform, const glm::vec4& color, uint32_t entityId = 0);

    static void s_drawText(const std::string&  text,
                           const Font::Format& fontFormat,
                           const glm::vec3&    position,
                           const glm::vec2&    size,
                           uint32_t            entityId = 0);

    static void s_drawText(const std::string&  text,
                           const Font::Format& fontFormat,
                           const glm::mat4&    transform,
                           uint32_t            entityId = 0);

    static void s_resetStats();

    inline static Stats s_getStats()
    {
        return s_stats;
    }

   private:
    ///////////////////////////
    // Generic Data
    ///////////////////////////
    static bool s_inScene;

    ///////////////////////////
    //  Quad layout and data
    ///////////////////////////
    inline static const uint32_t k_quadInitCount = 25000;
    inline static const uint32_t k_quadGrowCount = 5000;
    inline static const uint32_t k_quadMaxCount  = 100000;

    inline static const BufferFormat k_quadVertexFormat = {
        {k_shaderVec4, "position"},
        {k_shaderVec2, "texCoords"},
    };

    struct QuadVertex
    {
        glm::vec4 position;
        glm::vec2 texCoord;
    };

    inline static const BufferFormat k_quadInstVertexFormat = {
        {k_shaderMat4, "transform", BufferComponent::Type::PER_INSTANCE, 1},
        {k_shaderVec4, "color", BufferComponent::Type::PER_INSTANCE, 1},
        {k_shaderInt, "texIndex", BufferComponent::Type::PER_INSTANCE, 1},
        {k_shaderFloat, "texTilingFactor", BufferComponent::Type::PER_INSTANCE, 1},
        {k_shaderUInt, "entityId", BufferComponent::Type::PER_INSTANCE, 1},
    };

    struct QuadInstVertex
    {
        glm::mat4 transform;
        glm::vec4 color;
        int32_t   texIndex;
        float     texTilingFactor;
        uint32_t  entityId;
    };

    struct QuadData
    {
        std::vector<QuadVertex>     vertices;
        std::vector<QuadInstVertex> instVertices;
        ref<VertexBuffer>           p_vbo     = nullptr;
        ref<VertexArray>            p_vao     = nullptr;
        ref<Shader>                 p_shader  = nullptr;
        uint32_t                    quadCount = 0;
        std::vector<ref<Texture>>   textures;
        bool                        needsResize = false;
    };

    static QuadData* s_quadData;

    ///////////////////////////
    //  Text layout and data
    ///////////////////////////
    inline static const uint32_t k_textVerticesInitCount = 500;
    inline static const uint32_t k_textVerticesGrowCount = 500;
    inline static const uint32_t k_textVerticesMaxCount  = 10000;

    inline static const BufferFormat k_TextVertexFormat = {
        {k_shaderVec4, "position"},
        {k_shaderVec2, "texCoords"},
        {k_shaderVec4, "fgColor"},
        {k_shaderVec4, "bgColor"},
        {k_shaderVec2, "unitRange"},
        {k_shaderInt, "texIndex"},
        {k_shaderUInt, "entityId"},
    };

    struct TextVertex
    {
        glm::vec4 position;
        glm::vec2 texCoord;
        glm::vec4 fgColor;
        glm::vec4 bgColor;
        glm::vec2 unitRange;
        int32_t   texIndex;
        uint32_t  entityId;
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

    static TextData* s_textData;

    static Stats s_stats;

    ///////////////////////////
    // Private functions
    ///////////////////////////
    static void _s_submit();
    static void _s_createTextBuffers();
    static void _s_createQuadBuffers();
};
}  // namespace nimbus