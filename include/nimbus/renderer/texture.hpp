#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/core/ref.hpp"

#include <cstdint>
#include <string>


namespace nimbus
{

class Texture : public refCounted
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

    enum class Format
    {
        NONE,
        RGBA,
        RGB,
        RG,
        RED,
        RED_INT
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
        R16,
        R8I,
        R16I,
        R32I,
        R8UI,
        R16UI,
        R32UI,
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
        MIPMAP_LINEAR,
        NEAREST,
    };

    enum class WrapType
    {
        CLAMP_TO_EDGE,
        REPEAT
    };

    struct Spec
    {
        uint32_t       width          = 1;
        uint32_t       height         = 1;   
        uint32_t       samples        = 1;
        Format         format         = Format::RGBA;
        FormatInternal formatInternal = FormatInternal::RGBA8;
        DataType       dataType       = DataType::UNSIGNED_BYTE;
        FilterType     filterTypeMin  = FilterType::LINEAR;
        FilterType     filterTypeMag  = FilterType::LINEAR;
        WrapType       wrapTypeS      = WrapType::CLAMP_TO_EDGE;
        WrapType       wrapTypeT      = WrapType::CLAMP_TO_EDGE;
        WrapType       wrapTypeR      = WrapType::CLAMP_TO_EDGE;
    };

    static ref<Texture> s_create(const Type type,
                                 Spec&      spec,
                                 bool       submitForMe = true)  noexcept;

    virtual ~Texture() = default;

    virtual void bind(const uint32_t glTextureUnit) const = 0;

    virtual void unbind() const = 0;

    virtual void setData(void* data, uint32_t size) = 0;

    virtual uint32_t getId() const = 0;
    
    virtual uint32_t getWidth() const = 0;

    virtual uint32_t getHeight() const = 0;

    virtual Type getType() const = 0;;

    virtual const std::string& getPath() const = 0;

    virtual const Spec& getSpec() const = 0;

    virtual bool operator==(const Texture& other) const = 0;

    bool isLoaded() const
    {
        return m_loaded;
    }

    static void s_setMaxTextures(uint32_t maxTextures) noexcept;

    static uint32_t s_getMaxTextures() noexcept;

    static uint32_t s_format(Format format) noexcept;

    static uint32_t s_formatInternal(FormatInternal format) noexcept;

    static uint32_t s_dataType(DataType dataType) noexcept;

    static uint32_t s_filterType(FilterType filterType) noexcept;

    static uint32_t s_wrapType(WrapType wrapType) noexcept;

   protected:
    Type    m_type;
    Spec    m_spec;

    uint32_t    m_id;
    std::string m_path;
    bool        m_flipOnLoad;
    bool        m_loaded = false;

    static const uint32_t k_maxTexturesUninit = 0;
    inline static uint32_t s_maxTextures = k_maxTexturesUninit;

   private:
    // ensure only Resource manager can call this
    static ref<Texture> s_create(const Type         type,
                                 const std::string& path,
                                 const bool         flipOnLoad)  noexcept;

    friend class ResourceManager;
};

}  // namespace nimbus