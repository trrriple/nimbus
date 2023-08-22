#pragma once
#include "nimbus/core/common.hpp"

#include <cstdint>
#include <string>
#include <variant>
#include <array>

namespace nimbus
{

class Texture : public refCounted
{
   public:
    enum class Type
    {
        diffuse,
        specular,
        ambient,
        normal,
        height
    };

    enum class Format
    {
        none,
        rgba,
        rgb,
        rg,
        red,
        redInt
    };

    enum class FormatInternal
    {
        none,
        rgba8,
        rgba16f,
        rgba32f,
        rgb8,
        rgb16f,
        rgb32f,
        rg8,
        rg16f,
        rg32f,
        r8,
        r16,
        r8i,
        r16i,
        r32i,
        r8ui,
        r16ui,
        r32ui,
        r16f,
        r32f,
        depthComponent16,
        depthComponent24,
        depthComponent32f,
        depth24Stencil8,
    };

    enum class DataType
    {
        unsignedByte_,
        byte_,
        unsignedShort_,
        short_,
        unsignedInt_,
        int_,
        float_,
        halfFloat_,
    };

    enum class FilterType
    {
        linear,
        mipmapLinear,
        nearest
    };

    enum class WrapType
    {
        clampToEdge,
        repeat
    };

    struct Spec
    {
        u32_t          width          = 1;
        u32_t          height         = 1;
        u32_t          samples        = 1;
        Format         format         = Format::rgba;
        FormatInternal formatInternal = FormatInternal::rgba8;
        DataType       dataType       = DataType::unsignedByte_;
        FilterType     filterTypeMin  = FilterType::linear;
        FilterType     filterTypeMag  = FilterType::linear;
        WrapType       wrapTypeS      = WrapType::clampToEdge;
        WrapType       wrapTypeT      = WrapType::clampToEdge;
        WrapType       wrapTypeR      = WrapType::clampToEdge;
        std::variant<std::array<f32_t, 4>, std::array<i32_t, 4>, std::array<u32_t, 4>> clearColor
            = std::array<f32_t, 4>{0.0f, 0.0f, 0.0f, 0.0f};
    };

    static ref<Texture> s_create(const Type type, Spec& spec, bool submitForMe = true);

    virtual ~Texture() = default;

    virtual bool bind(const u32_t glTextureUnit) const = 0;

    virtual void unbind() const = 0;

    virtual void setData(void* data, u32_t size) = 0;

    virtual u32_t getId() const = 0;

    virtual u32_t getWidth() const = 0;

    virtual u32_t getHeight() const = 0;

    virtual Type getType() const = 0;
    ;

    virtual const std::string& getPath() const = 0;

    virtual const Spec& getSpec() const = 0;

    virtual bool operator==(const Texture& other) const = 0;

    bool isLoaded() const
    {
        return m_loaded;
    }

    static void s_setMaxTextures(u32_t maxTextures);

    static u32_t s_getMaxTextures();

    static u32_t s_format(Format format);

    static u32_t s_formatInternal(FormatInternal format);

    static u32_t s_dataType(DataType dataType);

    static u32_t s_filterType(FilterType filterType);

    static u32_t s_wrapType(WrapType wrapType);

   protected:
    Type m_type;
    Spec m_spec;

    u32_t       m_id;
    std::string m_path;
    bool        m_flipOnLoad;
    bool        m_loaded = false;

    static const u32_t  k_maxTexturesUninit = 0;
    inline static u32_t s_maxTextures       = k_maxTexturesUninit;

   private:
    // ensure only Resource manager can call this
    static ref<Texture> s_create(const Type type, const std::string& path, const bool flipOnLoad);

    friend class ResourceManager;
};

}  // namespace nimbus