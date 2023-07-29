#include "nimbus/nmpch.hpp"
#include "nimbus/core.hpp"

#include "nimbus/renderer/texture.hpp"

#include "platform/gl/glTexture.hpp"

namespace nimbus
{
ref<Texture> Texture::s_create(const Type         type,
                               const std::string& path,
                               const bool         flipOnLoad)
{
    return makeRef<GlTexture>(type, path, flipOnLoad);
}

ref<Texture> Texture::s_create(const Type type, Spec& spec)
{
    return makeRef<GlTexture>(type, spec);
}

void Texture::s_setMaxTextures(uint32_t maxTextures)
{
    s_maxTextures = maxTextures;
}

uint32_t Texture::s_getMaxTextures()
{
    return s_maxTextures;
}

uint32_t Texture::s_format(Format format)
{
    return GlTexture::s_format(format);
}

uint32_t Texture::s_formatInternal(FormatInternal format)
{
    return GlTexture::s_formatInternal(format);
}

uint32_t Texture::s_dataType(DataType dataType)
{
    return GlTexture::s_dataType(dataType);
}

uint32_t Texture::s_filterType(FilterType filterType)
{
    return GlTexture::s_filterType(filterType);
}

uint32_t Texture::s_wrapType(WrapType wrapType)
{
    return GlTexture::s_wrapType(wrapType);
}

}  // namespace nimbus