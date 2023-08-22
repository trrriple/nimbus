#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/platform/gl/glTexture.hpp"
#include "nimbus/renderer/renderer.hpp"

#include "stb_image.h"
#include "glad.h"

namespace nimbus
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GlTexture::GlTexture(const Type type, const std::string& path, const bool flipOnLoad)
{
    NB_PROFILE();

    m_type       = type;
    m_path       = path;
    m_flipOnLoad = flipOnLoad;

    NB_CORE_ASSERT(!(s_maxTextures == k_maxTexturesUninit),
                   "s_maxTextures not initialized. Did you call "
                   "Texture::s_setMaxTextures?");

    stbi_set_flip_vertically_on_load(m_flipOnLoad);

    i32_t numComponents;

    u8_t* data = stbi_load(m_path.c_str(), (int*)&m_spec.width, (int*)&m_spec.height, &numComponents, 0);

    if (data)
    {
        if (numComponents == 1)
        {
            m_spec.format         = Format::red;
            m_spec.formatInternal = FormatInternal::r8;
        }
        else if (numComponents == 3)
        {
            m_spec.format         = Format::rgb;
            m_spec.formatInternal = FormatInternal::rgb8;
        }
        else if (numComponents == 4)
        {
            m_spec.format         = Format::rgba;
            m_spec.formatInternal = FormatInternal::rgba8;
        }
        else
        {
            NB_CORE_ASSERT(0, "Unknown image format has %i components", numComponents);
        }

        ref<GlTexture> p_this = this;

        Renderer::s_submitObject(
            [p_this, data]() mutable
            {
                _s_gen(p_this->m_id);
                glBindTexture(GL_TEXTURE_2D, p_this->m_id);

                // TODO, determine how to set this
                p_this->m_spec.dataType      = DataType::unsignedByte_;
                p_this->m_spec.filterTypeMin = FilterType::mipmapLinear;
                p_this->m_spec.filterTypeMag = FilterType::linear;
                p_this->m_spec.wrapTypeS     = WrapType::repeat;
                p_this->m_spec.wrapTypeT     = WrapType::repeat;
                p_this->m_spec.wrapTypeR     = WrapType::repeat;

                // safety check for:
                // If a non-zero named buffer object is bound to the
                // GL_PIXEL_UNPACK_BUFFER target (see glBindBuffer) while a
                // texture image is specified, data is treated as a byte offset
                // into the buffer object's data store.
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
                glTexImage2D(GL_TEXTURE_2D,
                             0,
                             s_formatInternal(p_this->m_spec.formatInternal),
                             p_this->m_spec.width,
                             p_this->m_spec.height,
                             0,
                             s_format(p_this->m_spec.format),
                             s_dataType(p_this->m_spec.dataType),
                             data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, s_filterType(p_this->m_spec.filterTypeMin));
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, s_filterType(p_this->m_spec.filterTypeMag));
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s_wrapType(p_this->m_spec.wrapTypeS));
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, s_wrapType(p_this->m_spec.wrapTypeT));
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, s_wrapType(p_this->m_spec.wrapTypeR));

                stbi_image_free(data);

                p_this->m_loaded = true;
            });

        // renderDoneFuture.wait();
    }
    else
    {
        Log::coreError("Texture failed to load at path: %s", m_path.c_str());
        stbi_image_free(data);
        m_loaded = false;
    }
}

GlTexture::GlTexture(const Type type, Spec& spec, bool submitForMe)
{
    m_type = type;
    m_spec = spec;

    NB_CORE_ASSERT(m_spec.samples, "Textures samples must be >= 1!");

    if (submitForMe)
    {
        ref<GlTexture> p_this = this;
        Renderer::s_submitObject([p_this]() mutable { p_this->_storage(); });
    }
    else
    {
        _storage();
    }
}

GlTexture::~GlTexture()
{
    u32_t id = m_id;
    Renderer::s_submitObject([id]() { glDeleteTextures(1, &id); });
}

bool GlTexture::bind(const u32_t glTextureUnit) const
{
    NB_CORE_ASSERT_STATIC((glTextureUnit <= s_maxTextures),
                          "glTextureUnit > s_setMaxTextures. Did you call "
                          "Texture::s_setMaxTextures?");

    if (!m_loaded)
    {
        return false;
    }

    ref<GlTexture> p_this = const_cast<GlTexture*>(this);

    Renderer::s_submit(
        [p_this, glTextureUnit]()
        {
            glActiveTexture(GL_TEXTURE0 + glTextureUnit);

            glBindTexture(p_this->m_spec.samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, p_this->m_id);
        });
    return true;
}

void GlTexture::unbind() const
{
    u32_t samples = m_spec.samples;
    Renderer::s_submit([samples]() { glBindTexture(samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, 0); });
}

void GlTexture::setData(void* data, u32_t size)
{
    u32_t elements        = 0;
    u32_t bytesPerElement = 0;
    switch (m_spec.format)
    {
        case (Format::rgba):
        {
            elements = 4;
            break;
        }
        case (Format::rgb):
        {
            elements = 3;
            break;
        }
        case (Format::rg):
        {
            elements = 2;
            break;
        }
        case (Format::red):
        {
            elements = 1;
            break;
        }
        default:
            NB_CORE_ASSERT(false, "Unknown texture format %i\n", m_spec.format);
    }

    switch (m_spec.dataType)
    {
        case (DataType::unsignedByte_):
        case (DataType::byte_):
        {
            bytesPerElement = 1;
            break;
        }
        case (DataType::unsignedShort_):
        case (DataType::short_):
        case (DataType::halfFloat_):
        {
            bytesPerElement = 2;
            break;
        }
        case (DataType::unsignedInt_):
        case (DataType::int_):
        case (DataType::float_):
        {
            bytesPerElement = 4;
            break;
        }
        default:
            NB_CORE_ASSERT(false, "Unknown texture data type %i\n", m_spec.dataType);
    }

    u32_t bytesPerPixel = elements * bytesPerElement;

    NB_CORE_ASSERT((size == m_spec.width * m_spec.height * bytesPerPixel),
                   "Data size (%i) must be equal to texture Size (W:%i * H:%i "
                   "* Bpp:%i = %i)",
                   size,
                   m_spec.width,
                   m_spec.height,
                   bytesPerPixel,
                   m_spec.width * m_spec.height * bytesPerPixel);

    void* localCpy = malloc(size);
    memcpy(localCpy, data, size);

    ref<GlTexture> p_this = this;

    Renderer::s_submitObject(
        [p_this, localCpy]()
        {
            glTextureSubImage2D(p_this->m_id,
                                0,
                                0,
                                0,
                                p_this->m_spec.width,
                                p_this->m_spec.height,
                                s_format(p_this->m_spec.format),
                                s_dataType(p_this->m_spec.dataType),
                                localCpy);

            free(localCpy);
        });
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
u32_t GlTexture::s_format(Format format)
{
    switch (format)
    {
        case Format::none:
            return 0;
        case Format::rgba:
            return GL_RGBA;
        case Format::rgb:
            return GL_RGB;
        case Format::rg:
            return GL_RG;
        case Format::red:
            return GL_RED;
        case Format::redInt:
            return GL_RED_INTEGER;
        default:
        {
            NB_CORE_ASSERT_STATIC(false, "Unsupported texture format %i", format);
            return 0;
        }
    }
}

u32_t GlTexture::s_formatInternal(FormatInternal format)
{
    switch (format)
    {
        case FormatInternal::none:
            return 0;
        case FormatInternal::rgba8:
            return GL_RGBA8;
        case FormatInternal::rgba16f:
            return GL_RGBA16F;
        case FormatInternal::rgba32f:
            return GL_RGBA32F;
        case FormatInternal::rgb8:
            return GL_RGB8;
        case FormatInternal::rgb16f:
            return GL_RGB16F;
        case FormatInternal::rgb32f:
            return GL_RGB32F;
        case FormatInternal::rg8:
            return GL_RG8;
        case FormatInternal::rg16f:
            return GL_RG16F;
        case FormatInternal::rg32f:
            return GL_RG32F;
        case FormatInternal::r8:
            return GL_R8;
        case FormatInternal::r16:
            return GL_R16;
        case FormatInternal::r8i:
            return GL_R8I;
        case FormatInternal::r16i:
            return GL_R16I;
        case FormatInternal::r32i:
            return GL_R32I;
        case FormatInternal::r8ui:
            return GL_R8UI;
        case FormatInternal::r16ui:
            return GL_R16UI;
        case FormatInternal::r32ui:
            return GL_R32UI;
        case FormatInternal::r32f:
            return GL_R32F;
        case FormatInternal::depthComponent16:
            return GL_DEPTH_COMPONENT16;
        case FormatInternal::depthComponent24:
            return GL_DEPTH_COMPONENT24;
        case FormatInternal::depthComponent32f:
            return GL_DEPTH_COMPONENT32F;
        case FormatInternal::depth24Stencil8:
            return GL_DEPTH24_STENCIL8;
        default:
        {
            NB_CORE_ASSERT_STATIC(false, "Unsupported internal texture format %i", format);
            return 0;
        }
    }
}

u32_t GlTexture::s_dataType(DataType dataType)
{
    switch (dataType)
    {
        case DataType::unsignedByte_:
            return GL_UNSIGNED_BYTE;
        case DataType::byte_:
            return GL_BYTE;
        case DataType::unsignedShort_:
            return GL_UNSIGNED_SHORT;
        case DataType::short_:
            return GL_SHORT;
        case DataType::unsignedInt_:
            return GL_UNSIGNED_INT;
        case DataType::int_:
            return GL_INT;
        case DataType::float_:
            return GL_FLOAT;
        case DataType::halfFloat_:
            return GL_HALF_FLOAT;
        // Add more conversions as needed
        default:
        {
            NB_CORE_ASSERT_STATIC(false, "Unsupported texture data type %i", dataType);
            return 0;
        }
    }
}

u32_t GlTexture::s_filterType(FilterType filterType)
{
    switch (filterType)
    {
        case (FilterType::linear):
            return GL_LINEAR;
        case (FilterType::mipmapLinear):
            return GL_LINEAR_MIPMAP_LINEAR;
        case (FilterType::nearest):
            return GL_NEAREST;
        default:
        {
            NB_CORE_ASSERT_STATIC(false, "Unknown Texture Filter Type %i", filterType);
            return 0;
        }
    }
}

u32_t GlTexture::s_wrapType(WrapType wrapType)
{
    switch (wrapType)
    {
        case (WrapType::clampToEdge):
            return GL_CLAMP_TO_EDGE;
        case (WrapType::repeat):
            return GL_REPEAT;
        default:
        {
            NB_CORE_ASSERT_STATIC(false, "Unknown Texture Wrap Type %i", wrapType);
            return 0;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GlTexture::_s_gen(u32_t& id, bool multisample)
{
    glCreateTextures(multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, 1, &id);
}

void GlTexture::_storage()
{
    if (m_spec.samples == 1)
    {
        _s_gen(m_id);

        glTextureStorage2D(m_id, 1, Texture::s_formatInternal(m_spec.formatInternal), m_spec.width, m_spec.height);

        glBindTexture(GL_TEXTURE_2D, m_id);

        glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, Texture::s_filterType(m_spec.filterTypeMag));

        glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, Texture::s_filterType(m_spec.filterTypeMin));

        glTextureParameteri(m_id, GL_TEXTURE_WRAP_R, Texture::s_wrapType(m_spec.wrapTypeR));
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, Texture::s_wrapType(m_spec.wrapTypeS));
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, Texture::s_wrapType(m_spec.wrapTypeT));

        m_loaded = true;

        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        _s_gen(m_id, true);
        glTextureStorage2DMultisample(m_id,
                                      m_spec.samples,
                                      Texture::s_formatInternal(m_spec.formatInternal),
                                      m_spec.width,
                                      m_spec.height,
                                      GL_TRUE);

        m_loaded = true;
    }
}

}  // namespace nimbus