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
    
    static void drawText(const std::string& text, const Font& font);

   private:
    ///////////////////////////
    //  Text layout and data
    ///////////////////////////
    inline static const uint32_t k_textVerticesInitCount = 1000;
    inline static const uint32_t k_textVerticesGrowCount = 500;
    inline static const uint32_t k_textVerticesMaxCount  = 10000;

    inline static const BufferFormat k_TextVertexFormat = {
        {k_shaderVec3, "position"},
    };
    struct TextVertex
    {
        glm::vec3 position;
    };

    inline static const BufferFormat k_TextVertexInstanceFormat = {
        {k_shaderVec3, "offset"},
        {k_shaderVec2, "texCoords"},
        {k_shaderVec4, "color"},
    };
    struct TextVertexInstance
    {
        glm::vec3 offset;
        glm::vec2 texCoord;
        glm::vec4 color;
    };

    struct TextData
    {
        std::vector<TextVertexInstance> vertices;
        ref<VertexBuffer>       p_vbo = nullptr;
        ref<VertexArray>        p_vao = nullptr;

        uint32_t charCount = 0;
    };

    static TextData s_textData;
};
}  // namespace nimbus