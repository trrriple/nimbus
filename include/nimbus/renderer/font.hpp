#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/renderer/texture.hpp"

namespace nimbus
{
struct FontData;

class Font
{
   public:
    struct Format
    {
        ref<Font> p_font      = nullptr;
        glm::vec4 fgColor     = glm::vec4(1.0f);
        glm::vec4 bgColor     = glm::vec4(0.0f);
        float     kerning     = 0.0f;
        float     lineSpacing = 0.0f;
    };

    Font(const std::string& fontPath);
    ~Font();

    const std::string& getPath() const
    {
        return m_path;
    }

    ref<Texture> getAtlasTex() const
    {
        return m_atlasTex;
    }

    const FontData* getFontData() const
    {
        return m_data;
    }

   private:
    std::string  m_path;
    FontData*    m_data;
    ref<Texture> m_atlasTex;
};

}  // namespace nimbus