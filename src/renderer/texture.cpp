
#include "nmpch.hpp"
#include "core.hpp"

#include "renderer/texture.hpp"
#include "stb_image.h"


namespace nimbus
{

Texture::Texture(const Type         type,
                 const std::string& path,
                 const bool         flipOnLoad)
    : m_type(type), m_path(path), m_flipOnLoad(flipOnLoad)
{
    NM_CORE_ASSERT(!(s_maxTextures == k_maxTexturesUninit),
                   "s_maxTextures not initialized. Did you call "
                   "Texture::s_setMaxTextures?\n");

    _load();
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_id);
}

void Texture::bind(const uint32_t glTextureUnit) const
{
    NM_CORE_ASSERT((glTextureUnit <= s_maxTextures),
                   "glTextureUnit > s_setMaxTextures. Did you call "
                   "Texture::s_setMaxTextures?\n");

    glActiveTexture(GL_TEXTURE0 + glTextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_id);
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
    }
}

void Texture::s_setMaxTextures(uint32_t maxTextures)
{
    s_maxTextures = maxTextures;
    _initializeUniformNames();
}

uint32_t Texture::s_getMaxTextures()
{
    return s_maxTextures;
}

void Texture::s_unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::_load()
{
    glGenTextures(1, &m_id);

    int32_t width;
    int32_t height;
    int32_t nrComponents;

    stbi_set_flip_vertically_on_load(m_flipOnLoad);

    uint8_t* data
        = stbi_load(m_path.c_str(), &width, &height, &nrComponents, 0);

    if (data)
    {
        GLenum format = 0;
        if (nrComponents == 1)
        {
            format = GL_RED;
        }
        else if (nrComponents == 3)
        {
            format = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            format = GL_RGBA;
        }
        else    
        {
            NM_CORE_ASSERT(0, "Unknown image format 0x%x\n", format);
        }

        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     format,
                     width,
                     height,
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
        NM_CORE_ERROR("Texture failed to load at path: %s\n", m_path.c_str());
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