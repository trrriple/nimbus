#pragma once

#include <cstdint>
#include <mutex>
#include <string>

namespace nimbus
{

class Texture
{
    static const uint32_t k_maxTexturesUninit = 0;

   public:
    enum class Type
    {
        DIFFUSE,
        SPECULAR,
        AMBIENT,
        NORMAL,
        HEIGHT
    };

    enum class TexFormat
    {
        NONE,
        RGBA,
        RGB,
        RG,
        RED,
    };

    enum class TexFormatInternal
    {
        NONE,
        RGBA8,
        RGBA16F,
        RGBA32F,
        RGB8,
        RGB16F,
        RGB32F,
        RG8,
        RG16F,
        RG32F,
        R8,
        R16F,
        R32F,
        DEPTH_COMPONENT16,
        DEPTH_COMPONENT24,
        DEPTH_COMPONENT32F,
        DEPTH24_STENCIL8,
    };

    enum class TexDataType
    {
        UNSIGNED_BYTE,
        BYTE,
        UNSIGNED_SHORT,
        SHORT,
        UNSIGNED_INT,
        INT,
        FLOAT,
        HALF_FLOAT,
    };

    enum class TexFilterType
    {
        LINEAR,
        MIPMAP_LINEAR
    };

    enum class TexWrapType
    {
        CLAMP_TO_EDGE,
        REPEAT
    };

    struct TextureSpec
    {
        TexFormat         format         = TexFormat::RGBA;
        TexFormatInternal formatInternal = TexFormatInternal::RGBA8;
        TexDataType       dataType       = TexDataType::UNSIGNED_BYTE;
        TexFilterType     filterTypeMin  = TexFilterType::LINEAR;
        TexFilterType     filterTypeMag  = TexFilterType::LINEAR;
        TexWrapType       wrapTypeS      = TexWrapType::CLAMP_TO_EDGE;
        TexWrapType       wrapTypeT      = TexWrapType::CLAMP_TO_EDGE;
        TexWrapType       wrapTypeR      = TexWrapType::CLAMP_TO_EDGE;
    };


    Texture(const Type         type,
            const std::string& path,
            const bool         flipOnLoad = false);

    ~Texture();

    void bind(const uint32_t glTextureUnit) const;

    Type getType() const
    {
        return m_type;
    }

    const std::string& getPath() const
    {
        return m_path;
    }
    
    const TextureSpec& getSpec() const
    {
        return m_spec;
    }

    static void s_setMaxTextures(uint32_t maxTextures);

    static uint32_t s_getMaxTextures();

    static void s_bind(const uint32_t textureId,
                       const uint32_t glTextureUnit,
                       bool           multisample = false);

    static void s_unbind(bool multisample = false);

    static void s_gen(uint32_t& id, bool multisample = false);

    static uint32_t s_texFormat(TexFormat format);

    static uint32_t s_texFormatInternal(TexFormatInternal format);

    static uint32_t s_texDataType(TexDataType dataType);

    static uint32_t s_texFilterType(TexFilterType filterType);

    static uint32_t s_texWrapType(TexWrapType wrapType);

   private:
    TextureSpec m_spec;
    uint32_t    m_id;
    Type        m_type;
    std::string m_path;
    bool        m_flipOnLoad;
    int32_t     m_height;
    int32_t     m_width;

    inline static uint32_t                 s_maxTextures = k_maxTexturesUninit;

    inline static std::uint32_t            s_currBoundId          = 0;
    inline static std::uint32_t            s_currBoundTextureUnit = 0;
    inline static std::mutex               s_genLock = std::mutex();
};

}  // namespace nimbus