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

    enum class Format
    {
        NONE,
        RGBA,
        RGB,
        RG,
        RED,
    };

    enum class FormatInternal
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

    enum class DataType
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

    enum class FilterType
    {
        LINEAR,
        MIPMAP_LINEAR
    };

    enum class WrapType
    {
        CLAMP_TO_EDGE,
        REPEAT
    };

    struct Spec
    {
        Format         format         = Format::RGBA;
        FormatInternal formatInternal = FormatInternal::RGBA8;
        DataType       dataType       = DataType::UNSIGNED_BYTE;
        FilterType     filterTypeMin  = FilterType::LINEAR;
        FilterType     filterTypeMag  = FilterType::LINEAR;
        WrapType       wrapTypeS      = WrapType::CLAMP_TO_EDGE;
        WrapType       wrapTypeT      = WrapType::CLAMP_TO_EDGE;
        WrapType       wrapTypeR      = WrapType::CLAMP_TO_EDGE;
    };

    Texture(const Type         type,
            const std::string& path,
            const bool         flipOnLoad = false);

    Texture(const Type type, uint32_t width, uint32_t height, Spec& spec);

    ~Texture();

    void bind(const uint32_t glTextureUnit) const;

    void setData(void* data, uint32_t size);

    uint32_t getId() const
    {
        return m_id;
    }
    
    uint32_t getWidth() const
    {
        return m_width;
    }

  uint32_t getHeight() const
    {
        return m_height;
    }


    Type getType() const
    {
        return m_type;
    }

    const std::string& getPath() const
    {
        return m_path;
    }

    const Spec& getSpec() const
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

    static uint32_t s_format(Format format);

    static uint32_t s_formatInternal(FormatInternal format);

    static uint32_t s_dataType(DataType dataType);

    static uint32_t s_filterType(FilterType filterType);

    static uint32_t s_wrapType(WrapType wrapType);

   private:
    Type        m_type;
    int32_t     m_width;
    int32_t     m_height;
    Spec        m_spec;

    uint32_t    m_id;
    std::string m_path;
    bool        m_flipOnLoad;

    inline static uint32_t s_maxTextures = k_maxTexturesUninit;

    inline static std::uint32_t s_currBoundId          = 0;
    inline static std::uint32_t s_currBoundTextureUnit = 0;
    inline static std::mutex    s_genLock              = std::mutex();
};

}  // namespace nimbus