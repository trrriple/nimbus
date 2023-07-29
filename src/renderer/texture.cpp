#include "nmpch.hpp"
#include "core.hpp"

#include "renderer/texture.hpp"

#include "stb_image.h"

namespace nimbus
{

////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////
Texture::Texture(const Type         type,
                 const std::string& path,
                 const bool         flipOnLoad)
    : m_type(type), m_path(path), m_flipOnLoad(flipOnLoad)
{
    NM_PROFILE();

    NM_CORE_ASSERT(!(s_maxTextures == k_maxTexturesUninit),
                   "s_maxTextures not initialized. Did you call "
                   "Texture::s_setMaxTextures?");

    s_gen(m_id);

    stbi_set_flip_vertically_on_load(m_flipOnLoad);

    int32_t numComponents;

    uint8_t* data
        = stbi_load(m_path.c_str(), &m_width, &m_height, &numComponents, 0);

    if (data)
    {
        if (numComponents == 1)
        {
            m_spec.format         = Format::RED;
            m_spec.formatInternal = FormatInternal::R8;
        }
        else if (numComponents == 3)
        {
            m_spec.format         = Format::RGB;
            m_spec.formatInternal = FormatInternal::RGB8;
        }
        else if (numComponents == 4)
        {
            m_spec.format         = Format::RGBA;
            m_spec.formatInternal = FormatInternal::RGBA8;
        }
        else
        {
            NM_CORE_ASSERT(
                0, "Unknown image format has %i components", numComponents);
        }

        glBindTexture(GL_TEXTURE_2D, m_id);

        // TODO, determine how to set this
        m_spec.dataType      = DataType::UNSIGNED_BYTE;
        m_spec.filterTypeMin = FilterType::MIPMAP_LINEAR;
        m_spec.filterTypeMag = FilterType::LINEAR;
        m_spec.wrapTypeS     = WrapType::REPEAT;
        m_spec.wrapTypeT     = WrapType::REPEAT;
        m_spec.wrapTypeR     = WrapType::REPEAT;

        // safety check for:
        // If a non-zero named buffer object is bound to the
        // GL_PIXEL_UNPACK_BUFFER target (see glBindBuffer) while a texture
        // image is specified, data is treated as a byte offset into the buffer
        // object's data store.
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     s_formatInternal(m_spec.formatInternal),
                     m_width,
                     m_height,
                     0,
                     s_format(m_spec.format),
                     s_dataType(m_spec.dataType),
                     data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_MIN_FILTER,
                        s_filterType(m_spec.filterTypeMin));
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_MAG_FILTER,
                        s_filterType(m_spec.filterTypeMag));
        glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s_wrapType(m_spec.wrapTypeS));
        glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, s_wrapType(m_spec.wrapTypeT));
        glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, s_wrapType(m_spec.wrapTypeR));

        stbi_image_free(data);
    }
    else
    {
        Log::coreError("Texture failed to load at path: %s", m_path.c_str());
        stbi_image_free(data);
    }
}

Texture::Texture(const Type type, uint32_t width, uint32_t height, Spec& spec)
    : m_type(type), m_width(width), m_height(height), m_spec(spec)
{
    s_gen(m_id);

    glTextureStorage2D(m_id,
                       1,
                       Texture::s_formatInternal(m_spec.formatInternal),
                       m_width,
                       m_height);

    glTextureParameteri(m_id,
                        GL_TEXTURE_MAG_FILTER,
                        Texture::s_filterType(m_spec.filterTypeMag));

    glTextureParameteri(m_id,
                        GL_TEXTURE_MIN_FILTER,
                        Texture::s_filterType(m_spec.filterTypeMin));

    glTextureParameteri(
        m_id, GL_TEXTURE_WRAP_R, Texture::s_wrapType(m_spec.wrapTypeR));
    glTextureParameteri(
        m_id, GL_TEXTURE_WRAP_S, Texture::s_wrapType(m_spec.wrapTypeS));
    glTextureParameteri(
        m_id, GL_TEXTURE_WRAP_T, Texture::s_wrapType(m_spec.wrapTypeT));
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_id);
}

void Texture::bind(const uint32_t glTextureUnit) const
{
    s_bind(m_id, glTextureUnit);
}

void Texture::setData(void* data, uint32_t size)
{
    uint32_t elements        = 0;
    uint32_t bytesPerElement = 0;
    switch (m_spec.format)
    {
        case (Format::RGBA):
        {
            elements = 4;
            break;
        }
        case (Format::RGB):
        {
            elements = 3;
            break;
        }
        case (Format::RG):
        {
            elements = 2;
            break;
        }
        case (Format::RED):
        {
            elements = 1;
            break;
        }
        default:
            NM_CORE_ASSERT(false, "Unknown texture format %i\n", m_spec.format);
    }

    switch (m_spec.dataType)
    {
        case (DataType::UNSIGNED_BYTE):
        case (DataType::BYTE):
        {
            bytesPerElement = 1;
            break;
        }
        case (DataType::UNSIGNED_SHORT):
        case (DataType::SHORT):
        case (DataType::HALF_FLOAT):
        {
            bytesPerElement = 2;
            break;
        }
        case (DataType::UNSIGNED_INT):
        case (DataType::INT):
        case (DataType::FLOAT):
        {
            bytesPerElement = 4;
            break;
        }
        default:
            NM_CORE_ASSERT(
                false, "Unknown texture data type %i\n", m_spec.dataType);
    }

    uint32_t bytesPerPixel = elements * bytesPerElement;

    NM_CORE_ASSERT((size == m_width * m_height * bytesPerPixel),
                   "Data size (%i) must be equal to texture Size (W:%i * H:%i "
                   "* Bpp:%i = %i)",
                   size,
                   m_width,
                   m_height,
                   bytesPerPixel,
                   m_width * m_height * bytesPerPixel);

    glTextureSubImage2D(m_id,
                        0,
                        0,
                        0,
                        m_width,
                        m_height,
                        s_format(m_spec.format),
                        s_dataType(m_spec.dataType),
                        data);
}

bool Texture::operator==(const Texture& other) const
{
    return (m_id == other.m_id);
}

////////////////////////////////////////////////////////////////////////////////
// Static Functions
////////////////////////////////////////////////////////////////////////////////
void Texture::s_setMaxTextures(uint32_t maxTextures)
{
    s_maxTextures = maxTextures;
}

uint32_t Texture::s_getMaxTextures()
{
    return s_maxTextures;
}

void Texture::s_bind(const uint32_t textureId,
                     const uint32_t glTextureUnit,
                     bool           multisample)
{
    NM_PROFILE_TRACE();

    NM_CORE_ASSERT_STATIC((glTextureUnit <= s_maxTextures),
                          "glTextureUnit > s_setMaxTextures. Did you call "
                          "Texture::s_setMaxTextures?");

    if (glTextureUnit != s_currBoundTextureUnit)
    {
        glActiveTexture(GL_TEXTURE0 + glTextureUnit);
        s_currBoundTextureUnit = glTextureUnit;
    }

    if (textureId != s_currBoundId)
    {
        glBindTexture(multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D,
                      textureId);
        s_currBoundId = textureId;
    }
}

void Texture::s_unbind(bool multisample)
{
    NM_PROFILE_TRACE();

    glBindTexture(multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, 0);
    s_currBoundId = 0;
}

void Texture::s_gen(uint32_t& id, bool multisample)
{
    std::lock_guard<std::mutex> lock(s_genLock);
    glCreateTextures(
        multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, 1, &id);
}

uint32_t Texture::s_format(Format format)
{
    switch (format)
    {
        case Format::NONE:
            return 0;
        case Format::RGBA:
            return GL_RGBA;
        case Format::RGB:
            return GL_RGB;
        case Format::RG:
            return GL_RG;
        case Format::RED:
            return GL_RED;
        default:
        {
            NM_CORE_ASSERT_STATIC(
                false, "Unsupported texture format %i", format);
            return 0;
        }
    }
}

uint32_t Texture::s_formatInternal(FormatInternal format)
{
    switch (format)
    {
        case FormatInternal::NONE:
            return 0;
        case FormatInternal::RGBA8:
            return GL_RGBA8;
        case FormatInternal::RGBA16F:
            return GL_RGBA16F;
        case FormatInternal::RGBA32F:
            return GL_RGBA32F;
        case FormatInternal::RGB8:
            return GL_RGB8;
        case FormatInternal::RGB16F:
            return GL_RGB16F;
        case FormatInternal::RGB32F:
            return GL_RGB32F;
        case FormatInternal::RG8:
            return GL_RG8;
        case FormatInternal::RG16F:
            return GL_RG16F;
        case FormatInternal::RG32F:
            return GL_RG32F;
        case FormatInternal::R8:
            return GL_R8;
        case FormatInternal::R16F:
            return GL_R16F;
        case FormatInternal::R32F:
            return GL_R32F;
        case FormatInternal::DEPTH_COMPONENT16:
            return GL_DEPTH_COMPONENT16;
        case FormatInternal::DEPTH_COMPONENT24:
            return GL_DEPTH_COMPONENT24;
        case FormatInternal::DEPTH_COMPONENT32F:
            return GL_DEPTH_COMPONENT32F;
        case FormatInternal::DEPTH24_STENCIL8:
            return GL_DEPTH24_STENCIL8;
        default:
        {
            NM_CORE_ASSERT_STATIC(
                false, "Unsupported internal texture format %i", format);
            return 0;
        }
    }
}

uint32_t Texture::s_dataType(DataType dataType)
{
    switch (dataType)
    {
        case DataType::UNSIGNED_BYTE:
            return GL_UNSIGNED_BYTE;
        case DataType::BYTE:
            return GL_BYTE;
        case DataType::UNSIGNED_SHORT:
            return GL_UNSIGNED_SHORT;
        case DataType::SHORT:
            return GL_SHORT;
        case DataType::UNSIGNED_INT:
            return GL_UNSIGNED_INT;
        case DataType::INT:
            return GL_INT;
        case DataType::FLOAT:
            return GL_FLOAT;
        case DataType::HALF_FLOAT:
            return GL_HALF_FLOAT;
        // Add more conversions as needed
        default:
        {
            NM_CORE_ASSERT_STATIC(
                false, "Unsupported texture data type %i", dataType);
            return 0;
        }
    }
}

uint32_t Texture::s_filterType(FilterType filterType)
{
    switch (filterType)
    {
        case (FilterType::LINEAR):
            return GL_LINEAR;
        case (FilterType::MIPMAP_LINEAR):
            return GL_LINEAR_MIPMAP_LINEAR;
        default:
        {
            NM_CORE_ASSERT_STATIC(
                false, "Unknown Texture Filter Type %i", filterType);
            return 0;
        }
    }
}

uint32_t Texture::s_wrapType(WrapType wrapType)
{
    switch (wrapType)
    {
        case (WrapType::CLAMP_TO_EDGE):
            return GL_CLAMP_TO_EDGE;
        case (WrapType::REPEAT):
            return GL_REPEAT;
        default:
        {
            NM_CORE_ASSERT_STATIC(
                false, "Unknown Texture Wrap Type %i", wrapType);
            return 0;
        }
    }
}

}  // namespace nimbus