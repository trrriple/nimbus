
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
    NM_CORE_ASSERT(!(s_maxTextures == k_maxTexturesUninit),
                   "s_maxTextures not initialized. Did you call "
                   "Texture::s_setMaxTextures?");

    _load();
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_id);
}

void Texture::bind(const uint32_t glTextureUnit) const
{
    s_bind(m_id, glTextureUnit);
}

const std::string& Texture::getUniformNm(uint32_t index) const
{
    switch (m_type)
    {
        case (Type::DIFFUSE):
        {
            return s_texDiffUniformNms[index];
        }
        case (Type::SPECULAR):
        {
            return s_texSpecUniformNms[index];
        }
        case (Type::AMBIENT):
        {
            return s_texAmbiUniformNms[index];
        }
        case (Type::NORMAL):
        {
            return s_texNormUniformNms[index];
        }
        case (Type::HEIGHT):
        {
            return s_texHghtUniformNms[index];
        }
        default:
        {
            NM_CORE_ASSERT(false, "Unsupported texture type!");
            return s_texDiffUniformNms[index];
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Static Functions
////////////////////////////////////////////////////////////////////////////////
void Texture::s_setMaxTextures(uint32_t maxTextures)
{
    s_maxTextures = maxTextures;
    _initializeUniformNames();
}

uint32_t Texture::s_getMaxTextures()
{
    return s_maxTextures;
}

void Texture::s_bind(const uint32_t textureId,
                     const uint32_t glTextureUnit,
                     bool           multisample)
{
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
    glBindTexture(multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, 0);
    s_currBoundId = 0;
}

void Texture::s_gen(uint32_t& id, bool multisample)
{
    std::lock_guard<std::mutex> lock(s_genLock);
    glCreateTextures(
        multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, 1, &id);
}


////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////
void Texture::_load()
{
    s_gen(m_id);

    stbi_set_flip_vertically_on_load(m_flipOnLoad);

    uint8_t* data
        = stbi_load(m_path.c_str(), &m_width, &m_height, &m_numComponents, 0);

    if (data)
    {
        GLenum format = 0;
        if (m_numComponents == 1)
        {
            format = GL_RED;
        }
        else if (m_numComponents == 3)
        {
            format = GL_RGB;
        }
        else if (m_numComponents == 4)
        {
            format = GL_RGBA;
        }
        else
        {
            NM_CORE_ASSERT(0, "Unknown image format 0x%x", m_numComponents);
        }

        glBindTexture(GL_TEXTURE_2D, m_id);

        // safety check for:
        // If a non-zero named buffer object is bound to the
        // GL_PIXEL_UNPACK_BUFFER target (see glBindBuffer) while a texture
        // image is specified, data is treated as a byte offset into the buffer
        // object's data store.
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     format,
                     m_width,
                     m_height,
                     0,
                     format,
                     GL_UNSIGNED_BYTE,
                     data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        Log::coreError("Texture failed to load at path: %s", m_path.c_str());
        stbi_image_free(data);
    }
}


void Texture::_initializeUniformNames()
{
    /* initialize uniform names */
    s_texDiffUniformNms.clear();
    s_texSpecUniformNms.clear();
    s_texAmbiUniformNms.clear();
    s_texNormUniformNms.clear();
    s_texHghtUniformNms.clear();

    for (uint32_t i = 0; i < s_maxTextures; i++)
    {
        std::string name = k_texDiffNm + "_" + std::to_string(i);
        s_texDiffUniformNms.push_back(name);

        name = k_texSpecNm + "_" + std::to_string(i);
        s_texSpecUniformNms.push_back(name);

        name = k_texAmbiNm + "_" + std::to_string(i);
        s_texAmbiUniformNms.push_back(name);

        name = k_texNormNm + "_" + std::to_string(i);
        s_texNormUniformNms.push_back(name);

        name = k_texHghtNm + "_" + std::to_string(i);
        s_texHghtUniformNms.push_back(name);
    }
}

}  // namespace nimbus