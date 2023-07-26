#pragma once
#include "common.hpp"
#include "renderer/texture.hpp"

namespace nimbus
{

class Font
{
   struct MsdfData;


   public:
    Font(const std::string& fontPath);
    ~Font();

    ref<Texture> getAtlasTex()
    {
        return m_atlasTex;
    }

   private:
    ref<Texture> m_atlasTex;

    MsdfData* m_data;
};

}  // namespace nimbus