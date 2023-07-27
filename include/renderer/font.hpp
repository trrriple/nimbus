#pragma once
#include "common.hpp"
#include "renderer/texture.hpp"

namespace nimbus
{
struct FontData;

class Font
{
   public:
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