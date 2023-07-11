#pragma once

#include "core.hpp"
#include "shader.hpp"

namespace nimbus
{

static const uint32_t    k_maxTexturesUninit = 0;
static const std::string k_texDiffNm         = "texDiff";
static const std::string k_texSpecNm         = "texSpec";
static const std::string k_texAmbiNm         = "texAmbi";
static const std::string k_texNormNm         = "texNorm";
static const std::string k_texHghtNm         = "texHght";

class Texture
{
   public:
    enum class Type
    {
        DIFFUSE,
        SPECULAR,
        AMBIENT,
        NORMAL,
        HEIGHT
    };

    uint32_t    m_id;
    Type        m_type;
    std::string m_path;
    bool        m_flipOnLoad;

    Texture(const Type         type,
            const std::string& path,
            const bool         flipOnLoad = false);

    ~Texture();

    void bind(const uint32_t glTextureUnit) const;

    const std::string& getUniformNm(uint32_t index) const;

    static void s_setMaxTextures(uint32_t maxTextures);

    static uint32_t s_getMaxTextures();


    static void s_unbind();

   private:
    inline static uint32_t                 s_maxTextures = k_maxTexturesUninit;
    inline static std::vector<std::string> s_texDiffUniformNms;
    inline static std::vector<std::string> s_texSpecUniformNms;
    inline static std::vector<std::string> s_texAmbiUniformNms;
    inline static std::vector<std::string> s_texNormUniformNms;
    inline static std::vector<std::string> s_texHghtUniformNms;


    void _load();

    static void _initializeUniformNames();
};

}  // namespace nimbus