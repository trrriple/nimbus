
#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "platform/gl/glFramebuffer.hpp"

#include "nimbus/renderer/texture.hpp"
#include "nimbus/renderer/graphicsApi.hpp"
#include "nimbus/renderer/renderer.hpp"

#include "glad.h"

namespace nimbus
{

////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////
GlFramebuffer::GlFramebuffer(Framebuffer::Spec& spec)
{
    NM_PROFILE_DETAIL();

    m_spec = spec;

    if (m_spec.width == 0 || m_spec.height == 0
        || m_spec.width > Framebuffer::k_maxDimension
        || m_spec.height > Framebuffer::k_maxDimension)
    {
        NM_CORE_ASSERT(
            false,
            "Attempted to make null sized Framebuffer or Yuge (> %i) "
            "Framebuffer\n",
            Framebuffer::k_maxDimension);
    }

    NM_CORE_ASSERT(m_spec.samples, "Must have at least 1 sample ");

    _construct();
}

GlFramebuffer::~GlFramebuffer()
{
    NM_PROFILE_DETAIL();

    uint32_t id    = m_fbo;
    uint32_t rboId = m_rbo;

    Renderer::s_submitObject(
        [id, rboId]()
        {
            glDeleteFramebuffers(1, &id);
            glDeleteRenderbuffers(1, &rboId);
        });

    m_colorAttachments.clear();
}

void GlFramebuffer::resize(uint32_t width, uint32_t height)
{
    NM_PROFILE();

    if (width == 0 || height == 0 || width > Framebuffer::k_maxDimension
        || height > Framebuffer::k_maxDimension)
    {
        NM_CORE_ASSERT(
            false,
            "Attempted to make null sized Framebuffer or Yuge (> %i) "
            "Framebuffer",
            Framebuffer::k_maxDimension);
    }

    m_spec.width  = width;
    m_spec.height = height;

    // force colorAttachments to be the correct size also
    for (auto& attachment : m_spec.colorAttachments)
    {
        attachment.width  = width;
        attachment.height = height;
    }

    _construct();
}

void GlFramebuffer::blit(ref<Framebuffer> p_destination,
                         const uint32_t   srcAttachmentIdx,
                         const uint32_t   destAttachmentIdx) const
{
    NM_PROFILE();

    NM_CORE_ASSERT(m_fbo != p_destination->getId(), "Cannot blit to self!");

    if (srcAttachmentIdx >= m_spec.colorAttachments.size())
    {
        Log::coreError(
            "Can't blit from attachment %i, FBO only has %i attachments!",
            srcAttachmentIdx,
            m_spec.colorAttachments.size());
    }

    if (destAttachmentIdx >= p_destination->getSpec().colorAttachments.size())
    {
        Log::coreError(
            "Can't blit from attachment %i, FBO only has %i attachments!",
            destAttachmentIdx,
            p_destination->getSpec().colorAttachments.size());
    }

    ref<Framebuffer> p_source = const_cast<GlFramebuffer*>(this);

    Renderer::s_submit(
        [p_source, p_destination, srcAttachmentIdx, destAttachmentIdx]()
        {
            glNamedFramebufferReadBuffer(
                p_source->getId(), GL_COLOR_ATTACHMENT0 + srcAttachmentIdx);

            GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0 + destAttachmentIdx};

            glNamedFramebufferDrawBuffers(
                p_destination->getId(), 1, drawBuffers);

            glBlitNamedFramebuffer(
                p_source->getId(),
                p_destination->getId(),
                0,
                0,
                p_source->getSpec().colorAttachments[srcAttachmentIdx].width,
                p_source->getSpec().colorAttachments[srcAttachmentIdx].height,
                0,
                0,
                p_destination->getSpec()
                    .colorAttachments[destAttachmentIdx]
                    .width,
                p_destination->getSpec()
                    .colorAttachments[destAttachmentIdx]
                    .height,
                GL_COLOR_BUFFER_BIT,
                GL_NEAREST);
        });
}

void GlFramebuffer::bind(Mode mode) const
{
    NM_PROFILE_TRACE();

    uint32_t id     = m_fbo;
    uint32_t width  = m_spec.width;
    uint32_t height = m_spec.height;

    Renderer::s_submit(
        [id, mode, width, height]()
        {
            switch (mode)
            {
                case (Mode::READ_WRITE):
                {
                    glBindFramebuffer(GL_FRAMEBUFFER, id);
                    break;
                }
                case (Mode::READ):
                {
                    glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
                    break;
                }
                case (Mode::WRITE):
                {
                    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
                    break;
                }
            }

            glViewport(0, 0, width, height);
        });
}

void GlFramebuffer::unbind(Mode mode) const
{
    NM_PROFILE_TRACE();

    Renderer::s_submit(
        [mode]()
        {
            switch (mode)
            {
                case (Mode::READ_WRITE):
                {
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    break;
                }
                case (Mode::READ):
                {
                    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
                    break;
                }
                case (Mode::WRITE):
                {
                    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
                    break;
                }
            }
        });
}

void GlFramebuffer::bindTexture(const uint32_t textureUnit,
                                const uint32_t attachmentIdx) const
{
    if (attachmentIdx >= m_colorAttachments.size())
    {
        Log::coreWarn("Color attachment Index out of range! (%i >= %i)",
                      attachmentIdx,
                      m_colorAttachments.size());
        return;
    }

    m_colorAttachments[attachmentIdx]->bind(textureUnit);
}

void GlFramebuffer::unbindTexture(const uint32_t attachmentIdx) const
{
    if (attachmentIdx >= m_colorAttachments.size())
    {
        Log::coreWarn("Color attachment Index out of range! (%i >= %i)",
                      attachmentIdx,
                      m_colorAttachments.size());
        return;
    }

    m_colorAttachments[attachmentIdx]->unbind();
}

void GlFramebuffer::clearColorAttachment(const uint32_t attachmentIdx)
{
    NM_PROFILE_DETAIL();

    ref<GlFramebuffer> p_this = this;

    switch (m_colorAttachments[attachmentIdx]->getSpec().formatInternal)
    {
        case (Texture::FormatInternal::RGBA8):
        case (Texture::FormatInternal::RGBA16F):
        case (Texture::FormatInternal::RGBA32F):
        case (Texture::FormatInternal::RGB8):
        case (Texture::FormatInternal::RGB16F):
        case (Texture::FormatInternal::RGB32F):
        case (Texture::FormatInternal::RG8):
        case (Texture::FormatInternal::RG16F):
        case (Texture::FormatInternal::RG32F):
        case (Texture::FormatInternal::R8):
        case (Texture::FormatInternal::R16):
        case (Texture::FormatInternal::R16F):
        case (Texture::FormatInternal::R32F):
        {
            // floating types
            Renderer::s_submitObject(
                [p_this, attachmentIdx]()
                {
                    if (attachmentIdx >= p_this->m_colorAttachments.size())
                    {
                        Log::coreWarn(
                            "Color attachment Index out of range! (%i >= %i)",
                            attachmentIdx,
                            p_this->m_colorAttachments.size());
                        return;
                    }

                    const std::array<float, 4> clearColor
                        = std::get<std::array<float, 4>>(
                            p_this->m_colorAttachments[attachmentIdx]
                                ->getSpec()
                                .clearColor);

                    glClearNamedFramebufferfv(p_this->m_fbo,
                                              GL_COLOR,
                                              attachmentIdx,
                                              clearColor.data());
                });
            break;
        }
        case (Texture::FormatInternal::R8I):
        case (Texture::FormatInternal::R16I):
        case (Texture::FormatInternal::R32I):
        {
            // signed int
            Renderer::s_submitObject(
                [p_this, attachmentIdx]()
                {
                    if (attachmentIdx >= p_this->m_colorAttachments.size())
                    {
                        Log::coreWarn(
                            "Color attachment Index out of range! (%i >= %i)",
                            attachmentIdx,
                            p_this->m_colorAttachments.size());
                        return;
                    }

                    const std::array<int32_t, 4> clearColor
                        = std::get<std::array<int32_t, 4>>(
                            p_this->m_colorAttachments[attachmentIdx]
                                ->getSpec()
                                .clearColor);

                    glClearNamedFramebufferiv(p_this->m_fbo,
                                              GL_COLOR,
                                              attachmentIdx,
                                              clearColor.data());
                });
            break;
        }
        case (Texture::FormatInternal::R8UI):
        case (Texture::FormatInternal::R16UI):
        case (Texture::FormatInternal::R32UI):
        {
            // unsigned int
            Renderer::s_submitObject(
                [p_this, attachmentIdx]()
                {
                    if (attachmentIdx >= p_this->m_colorAttachments.size())
                    {
                        Log::coreWarn(
                            "Color attachment Index out of range! (%i >= %i)",
                            attachmentIdx,
                            p_this->m_colorAttachments.size());
                        return;
                    }

                    const std::array<uint32_t, 4> clearColor
                        = std::get<std::array<uint32_t, 4>>(
                            p_this->m_colorAttachments[attachmentIdx]
                                ->getSpec()
                                .clearColor);

                    glClearNamedFramebufferuiv(p_this->m_fbo,
                                               GL_COLOR,
                                               attachmentIdx,
                                               clearColor.data());
                });
            break;
        }
        default:
            NM_CORE_ASSERT(
                false,
                "Can't clear framebuffer attachment type %i",
                m_colorAttachments[attachmentIdx]->getSpec().formatInternal);
            break;
    }
}

void GlFramebuffer::clearDepthAttachment()
{
    ref<GlFramebuffer> p_this = this;

    switch (m_spec.depthType)
    {
        case (Texture::FormatInternal::DEPTH_COMPONENT16):
        case (Texture::FormatInternal::DEPTH_COMPONENT24):
        case (Texture::FormatInternal::DEPTH_COMPONENT32F):
        {
            // depth
            Renderer::s_submitObject(
                [p_this]()
                {
                    GLfloat depthClearValue = 1.0f;
                    glClearNamedFramebufferfv(
                        p_this->m_fbo, GL_DEPTH, 0, &depthClearValue);
                });
            break;
        }
        case (Texture::FormatInternal::DEPTH24_STENCIL8):
        {
            Renderer::s_submitObject(
                [p_this]()
                {
                    // depth & stencil
                    GLfloat depthClearValue   = 1.0f;
                    GLint   stencilClearValue = 0;
                    glClearNamedFramebufferfi(p_this->m_fbo,
                                              GL_DEPTH_STENCIL,
                                              0,
                                              depthClearValue,
                                              stencilClearValue);
                });
            break;
        }
        default:
            NM_CORE_ASSERT(false,
                           "Can't clear framebuffer depth attachment type %i",
                           m_spec.depthType);
            break;
    }
}

void GlFramebuffer::clearAllAttachments()
{
    for (uint32_t i = 0; i < m_colorAttachments.size(); i++)
    {
        clearColorAttachment(i);
    }
    if (m_spec.depthType != Texture::FormatInternal::NONE)
    {
        clearDepthAttachment();
    }
}

void GlFramebuffer::requestPixel(ref<PixelReadRequest> p_request)
{
    ref<GlFramebuffer> p_this = this;

    Renderer::s_submit(
        [p_this, p_request]() mutable
        {
            auto [idx, x, y] = p_request->getLocation();

            if (idx >= p_this->m_colorAttachments.size())
            {
                Log::coreWarn("Color attachment Index out of range! (%i >= %i)",
                              idx,
                              p_this->m_colorAttachments.size());
                return;
            }

            glBindFramebuffer(GL_READ_FRAMEBUFFER, p_this->m_fbo);

            glReadBuffer(GL_COLOR_ATTACHMENT0 + idx);

            Texture::Format format
                = p_this->m_spec.colorAttachments[idx].format;

            switch (format)
            {
                case (Texture::Format::RGBA):
                {
                    glm::vec4 data;
                    glReadPixels(
                        x,
                        y,
                        1,
                        1,
                        Texture::s_format(format),
                        Texture::s_dataType(
                            p_this->m_spec.colorAttachments[idx].dataType),
                        &data);
                    p_request->setValue(data);
                    break;
                }
                case (Texture::Format::RGB):
                {
                    glm::vec3 data;
                    glReadPixels(
                        x,
                        y,
                        1,
                        1,
                        Texture::s_format(format),
                        Texture::s_dataType(
                            p_this->m_spec.colorAttachments[idx].dataType),
                        &data);
                    p_request->setValue(glm::vec4({data, 0.0f}));
                    break;
                }
                case (Texture::Format::RG):
                {
                    glm::vec2 data;
                    glReadPixels(
                        x,
                        y,
                        1,
                        1,
                        Texture::s_format(format),
                        Texture::s_dataType(
                            p_this->m_spec.colorAttachments[idx].dataType),
                        &data);
                    p_request->setValue(glm::vec4({data, 0.0f, 0.0f}));
                    break;
                }
                case (Texture::Format::RED_INT):
                {
                    uint32_t data;
                    glReadPixels(
                        x,
                        y,
                        1,
                        1,
                        Texture::s_format(format),
                        Texture::s_dataType(
                            p_this->m_spec.colorAttachments[idx].dataType),
                        &data);
                    p_request->setValue(data);
                    break;
                }
                default:
                {
                    return;
                }
            }
        });
}

////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////
void GlFramebuffer::_construct()
{
    NM_PROFILE();

    ////////////////////////////////////////////////////////////////////////////
    // Remove these outside lamba otherwise these would not be delete
    // until new buffer textures are created because deleting these
    // is a render call
    ///////////////////////////////////////////////////////////////////////////
    if (m_fbo)
    {
        m_colorAttachments.clear();
    }

    ref<GlFramebuffer> p_this = this;
    Renderer::s_submitObject(
        [p_this]() mutable
        {
            ////////////////////////////////////////////////////////////////////
            // blow away existing frame buffer if we've already made it and
            // create new
            ////////////////////////////////////////////////////////////////////
            if (p_this->m_fbo)
            {
                glDeleteFramebuffers(1, &p_this->m_fbo);
                glDeleteRenderbuffers(1, &p_this->m_rbo);
            }

            glCreateFramebuffers(1, &p_this->m_fbo);

            GLenum colorAttachments[p_this->m_spec.colorAttachments.size()];

            for (uint32_t i = 0; i < p_this->m_spec.colorAttachments.size();
                 i++)
            {
                auto texSpec = p_this->m_spec.colorAttachments[i];
                ////////////////////////////////////////////////////////////////
                // generate and bind the texture in which to write
                ////////////////////////////////////////////////////////////////
                if (texSpec.samples != p_this->m_spec.samples)
                {
                    NM_CORE_ASSERT_STATIC(
                        false,
                        "Color attachment texture spec samples must "
                        "match framebuffer "
                        "spec samples!");
                }

                if (texSpec.width != p_this->m_spec.width
                    || texSpec.height != p_this->m_spec.height)
                {
                    NM_CORE_ASSERT_STATIC(
                        false,
                        "Color attachment texture dimensions must "
                        "match framebuffer "
                        "dimensions!");
                }

                ref<Texture> texture
                    = Texture::s_create(Texture::Type::DIFFUSE, texSpec, false);

                p_this->m_colorAttachments.push_back(texture);

                glNamedFramebufferTexture(p_this->m_fbo,
                                          GL_COLOR_ATTACHMENT0 + i,
                                          texture->getId(),
                                          0);

                colorAttachments[i] = GL_COLOR_ATTACHMENT0 + i;
            }

            ////////////////////////////////////////////////////////////////////
            // Tell GL To draw to all of our color attachments
            ////////////////////////////////////////////////////////////////////
            glNamedFramebufferDrawBuffers(p_this->m_fbo,
                                          p_this->m_colorAttachments.size(),
                                          colorAttachments);

            ////////////////////////////////////////////////////////////////////
            // use render buffer for depth and stencil
            ////////////////////////////////////////////////////////////////////

            if (Texture::s_formatInternal(p_this->m_spec.depthType))
            {
                glCreateRenderbuffers(1, &p_this->m_rbo);
                // allocate
                if (p_this->m_spec.samples == 1)
                {
                    glNamedRenderbufferStorage(
                        p_this->m_rbo,
                        Texture::s_formatInternal(p_this->m_spec.depthType),
                        p_this->m_spec.width,
                        p_this->m_spec.height);
                }
                else
                {
                    glNamedRenderbufferStorageMultisample(
                        p_this->m_rbo,
                        p_this->m_spec.samples,
                        Texture::s_formatInternal(p_this->m_spec.depthType),
                        p_this->m_spec.width,
                        p_this->m_spec.height);
                }

                glNamedFramebufferRenderbuffer(p_this->m_fbo,
                                               GL_DEPTH_STENCIL_ATTACHMENT,
                                               GL_RENDERBUFFER,
                                               p_this->m_rbo);
            }

            ////////////////////////////////////////////////////////////////////////////
            // Verify complete frame buffer
            ////////////////////////////////////////////////////////////////////////////
            uint32_t result
                = glCheckNamedFramebufferStatus(p_this->m_fbo, GL_FRAMEBUFFER);
            if (result != GL_FRAMEBUFFER_COMPLETE)
            {
                NM_CORE_ASSERT_STATIC(
                    false, "Incomplete framebuffer! Error 0x%X", result);

                Log::coreCritical("Incomplete framebuffer! Error 0x%X", result);
            }
        });
}

uint32_t GlFramebuffer::_textureTarget() const
{
    return m_spec.samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

}  // namespace nimbus