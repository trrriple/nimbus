
#include "renderer/texture.hpp"

#include "core.hpp"
#include "nmpch.hpp"
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
            m_spec.format         = TexFormat::RED;
            m_spec.formatInternal = TexFormatInternal::R8;
        }
        else if (numComponents == 3)
        {
            m_spec.format         = TexFormat::RGB;
            m_spec.formatInternal = TexFormatInternal::RGB8;
        }
        else if (numComponents == 4)
        {
            m_spec.format         = TexFormat::RGBA;
            m_spec.formatInternal = TexFormatInternal::RGBA8;
        }
        else
        {
            NM_CORE_ASSERT(
                0, "Unknown image format has %i components", numComponents);
        }

        glBindTexture(GL_TEXTURE_2D, m_id);

        // TODO, determine how to set this
        m_spec.dataType      = TexDataType::UNSIGNED_BYTE;
        m_spec.filterTypeMin = TexFilterType::MIPMAP_LINEAR;
        m_spec.filterTypeMag = TexFilterType::LINEAR;
        m_spec.wrapTypeS     = TexWrapType::REPEAT;
        m_spec.wrapTypeT     = TexWrapType::REPEAT;
        m_spec.wrapTypeR     = TexWrapType::REPEAT;
   
        // safety check for:
        // If a non-zero named buffer object is bound to the
        // GL_PIXEL_UNPACK_BUFFER target (see glBindBuffer) while a texture
        // image is specified, data is treated as a byte offset into the buffer
        // object's data store.
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     s_texFormatInternal(m_spec.formatInternal),
                     m_width,
                     m_height,
                     0,
                     s_texFormat(m_spec.format),
                     s_texDataType(m_spec.dataType),
                     data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_MIN_FILTER,
                        s_texFilterType(m_spec.filterTypeMin));
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_MAG_FILTER,
                        s_texFilterType(m_spec.filterTypeMag));
        glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s_texWrapType(m_spec.wrapTypeS));
        glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, s_texWrapType(m_spec.wrapTypeT));
        glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, s_texWrapType(m_spec.wrapTypeR));

        stbi_image_free(data);
    }
    else
    {
        Log::coreError("Texture failed to load at path: %s", m_path.c_str());
        stbi_image_free(data);
    }
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_id);
}

void Texture::bind(const uint32_t glTextureUnit) const
{
    s_bind(m_id, glTextureUnit);
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

uint32_t Texture::s_texFormat(TexFormat format)
{
    switch (format)
    {
        case TexFormat::NONE:
            return 0;
        case TexFormat::RGBA:
            return GL_RGBA;
        case TexFormat::RGB:
            return GL_RGB;
        case TexFormat::RG:
            return GL_RG;
        case TexFormat::RED:
            return GL_RED;
        default:
        {
            NM_CORE_ASSERT_STATIC(
                false, "Unsupported texture format %i", format);
            return 0;
        }
    }
}

uint32_t Texture::s_texFormatInternal(TexFormatInternal format)
{
    switch (format)
    {
        case TexFormatInternal::NONE:
            return 0;
        case TexFormatInternal::RGBA8:
            return GL_RGBA8;
        case TexFormatInternal::RGBA16F:
            return GL_RGBA16F;
        case TexFormatInternal::RGBA32F:
            return GL_RGBA32F;
        case TexFormatInternal::RGB8:
            return GL_RGB8;
        case TexFormatInternal::RGB16F:
            return GL_RGB16F;
        case TexFormatInternal::RGB32F:
            return GL_RGB32F;
        case TexFormatInternal::RG8:
            return GL_RG8;
        case TexFormatInternal::RG16F:
            return GL_RG16F;
        case TexFormatInternal::RG32F:
            return GL_RG32F;
        case TexFormatInternal::R8:
            return GL_R8;
        case TexFormatInternal::R16F:
            return GL_R16F;
        case TexFormatInternal::R32F:
            return GL_R32F;
        case TexFormatInternal::DEPTH_COMPONENT16:
            return GL_DEPTH_COMPONENT16;
        case TexFormatInternal::DEPTH_COMPONENT24:
            return GL_DEPTH_COMPONENT24;
        case TexFormatInternal::DEPTH_COMPONENT32F:
            return GL_DEPTH_COMPONENT32F;
        case TexFormatInternal::DEPTH24_STENCIL8:
            return GL_DEPTH24_STENCIL8;
        default:
        {
            NM_CORE_ASSERT_STATIC(
                false, "Unsupported internal texture format %i", format);
            return 0;
        }
    }
}

uint32_t Texture::s_texDataType(TexDataType dataType)
{
    switch (dataType)
    {
        case TexDataType::UNSIGNED_BYTE:
            return GL_UNSIGNED_BYTE;
        case TexDataType::BYTE:
            return GL_BYTE;
        case TexDataType::UNSIGNED_SHORT:
            return GL_UNSIGNED_SHORT;
        case TexDataType::SHORT:
            return GL_SHORT;
        case TexDataType::UNSIGNED_INT:
            return GL_UNSIGNED_INT;
        case TexDataType::INT:
            return GL_INT;
        case TexDataType::FLOAT:
            return GL_FLOAT;
        case TexDataType::HALF_FLOAT:
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

uint32_t Texture::s_texFilterType(TexFilterType filterType)
{
    switch (filterType)
    {
        case (TexFilterType::LINEAR):
            return GL_LINEAR;
        case (TexFilterType::MIPMAP_LINEAR):
            return GL_LINEAR_MIPMAP_LINEAR;
        default:
        {
            NM_CORE_ASSERT_STATIC(
                false, "Unknown Texture Filter Type %i", filterType);
            return 0;
        }
    }
}

uint32_t Texture::s_texWrapType(TexWrapType wrapType)
{
    switch (wrapType)
    {
        case (TexWrapType::CLAMP_TO_EDGE):
            return GL_CLAMP_TO_EDGE;
        case (TexWrapType::REPEAT):
            return GL_REPEAT;
        default:
        {
            NM_CORE_ASSERT_STATIC(false, "Unknown Texture Wrap Type %i", wrapType);
            return 0;
        }
    }
}

}  // namespace nimbus