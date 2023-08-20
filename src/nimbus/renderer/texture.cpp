#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/renderer/texture.hpp"

#include "platform/gl/glTexture.hpp"

namespace nimbus
{
ref<Texture> Texture::s_create(const Type type, const std::string& path, const bool flipOnLoad)
{
    return ref<GlTexture>::gen(type, path, flipOnLoad);
}

ref<Texture> Texture::s_create(const Type type, Spec& spec, bool submitForMe)
{
    return ref<GlTexture>::gen(type, spec, submitForMe);
}

void Texture::s_setMaxTextures(u32_t maxTextures)
{
    s_maxTextures = maxTextures;
}

u32_t Texture::s_getMaxTextures()
{
    return s_maxTextures;
}

u32_t Texture::s_format(Format format)
{
    return GlTexture::s_format(format);
}

u32_t Texture::s_formatInternal(FormatInternal format)
{
    return GlTexture::s_formatInternal(format);
}

u32_t Texture::s_dataType(DataType dataType)
{
    return GlTexture::s_dataType(dataType);
}

u32_t Texture::s_filterType(FilterType filterType)
{
    return GlTexture::s_filterType(filterType);
}

u32_t Texture::s_wrapType(WrapType wrapType)
{
    return GlTexture::s_wrapType(wrapType);
}

}  // namespace nimbus