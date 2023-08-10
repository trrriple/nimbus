#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"
#include "nimbus/core/ref.hpp"

#include "nimbus/renderer/texture.hpp"

#include "platform/gl/glTexture.hpp"

namespace nimbus
{
ref<Texture> Texture::s_create(const Type         type,
                               const std::string& path,
                               const bool         flipOnLoad) noexcept
{
    return ref<GlTexture>::gen(type, path, flipOnLoad);
}

ref<Texture> Texture::s_create(const Type type,
                               Spec&      spec,
                               bool       submitForMe) noexcept
{
    return ref<GlTexture>::gen(type, spec, submitForMe);
}

void Texture::s_setMaxTextures(uint32_t maxTextures)  noexcept
{
    s_maxTextures = maxTextures;
}

uint32_t Texture::s_getMaxTextures() noexcept
{
    return s_maxTextures;
}

uint32_t Texture::s_format(Format format) noexcept
{
    return GlTexture::s_format(format);
}

uint32_t Texture::s_formatInternal(FormatInternal format) noexcept
{
    return GlTexture::s_formatInternal(format);
}

uint32_t Texture::s_dataType(DataType dataType) noexcept
{
    return GlTexture::s_dataType(dataType);
}

uint32_t Texture::s_filterType(FilterType filterType) noexcept
{
    return GlTexture::s_filterType(filterType);
}

uint32_t Texture::s_wrapType(WrapType wrapType) noexcept
{
    return GlTexture::s_wrapType(wrapType);
}

}  // namespace nimbus