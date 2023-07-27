#pragma once

#include "camera.hpp"
#include "common.hpp"
#include "glm.hpp"
#include "platform/buffer.hpp"
#include "renderer/shader.hpp"
#include "renderer/font.hpp"


namespace nimbus
{
class Renderer2D
{  
    
   public:    
    static void init();
    static void destroy();

    static void setScene(Camera& camera);

    static void drawText(const std::string& text,
                         const Font&        font,
                         const glm::vec3&   pos);

   private:
    ///////////////////////////
    // Generic Data
    ///////////////////////////
    struct GeneralData
    {
        glm::vec4 quadVertexPositions[4];
        glm::vec2 quadTextureCoords[4];
    };

    static GeneralData s_genData;


    ///////////////////////////
    //  Text layout and data
    ///////////////////////////
    inline static const uint32_t k_textVerticesInitCount = 1000;
    inline static const uint32_t k_textVerticesGrowCount = 500;
    inline static const uint32_t k_textVerticesMaxCount  = 10000;

    inline static const BufferFormat k_TextVertexFormat = {
        {k_shaderVec4, "position"},
        {k_shaderVec2, "texCoords"},
        {k_shaderVec4, "fgColor"},
        {k_shaderVec4, "bgColor"},
        
    };
    struct TextVertex
    {
        glm::vec4 position;
        glm::vec2 texCoord;
        glm::vec4 fgColor;
        glm::vec4 bgColor;
        
    };

    struct TextData
    {
        std::vector<TextVertex> vertices;
        uint32_t                vertexIdx = 0;
        ref<VertexBuffer>       p_vbo     = nullptr;
        ref<VertexArray>        p_vao     = nullptr;
        ref<Shader>             p_shader  = nullptr;
        ref<Texture>            p_atlas   = nullptr;
        uint32_t                charCount = 0;
    };

    static TextData s_textData;
};
}  // namespace nimbus