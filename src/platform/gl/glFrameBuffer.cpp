
#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "platform/gl/glFrameBuffer.hpp"

#include "nimbus/renderer/texture.hpp"
#include "nimbus/renderer/graphicsApi.hpp"
#include "nimbus/renderer/renderer.hpp"

#include "glad.h"

namespace nimbus
{

////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////
GlFrameBuffer::GlFrameBuffer(FrameBuffer::Spec& spec) noexcept
{
    NM_PROFILE_DETAIL();

    m_spec = spec;

    if (m_spec.width == 0 || m_spec.height == 0
        || m_spec.width > FrameBuffer::k_maxDimension
        || m_spec.height > FrameBuffer::k_maxDimension)
    {
        NM_CORE_ASSERT(
            false,
            "Attempted to make null sized FrameBuffer or Yuge (> %i) "
            "FrameBuffer\n",
            FrameBuffer::k_maxDimension);
    }

    NM_CORE_ASSERT(m_spec.samples, "Must have at least 1 sample ");

    _construct();
}

GlFrameBuffer::~GlFrameBuffer() noexcept
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

    m_textures.clear();
}

void GlFrameBuffer::resize(uint32_t width, uint32_t height) noexcept
{
    NM_PROFILE();

    if (width == 0 || height == 0 || width > FrameBuffer::k_maxDimension
        || height > FrameBuffer::k_maxDimension)
    {
        NM_CORE_ASSERT(
            false,
            "Attempted to make null sized FrameBuffer or Yuge (> %i) "
            "FrameBuffer",
            FrameBuffer::k_maxDimension);
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

void GlFrameBuffer::blit(const FrameBuffer& destination,
                         const uint32_t     srcAttachmentIdx,
                         const uint32_t     destAttachmentIdx) const noexcept
{
    NM_PROFILE();

    uint32_t id1 = m_fbo;
    uint32_t id2 = destination.getId();

    NM_CORE_ASSERT(id1 != id2, "Cannot blit to self!");

    if (srcAttachmentIdx >= m_spec.colorAttachments.size())
    {
        Log::coreError(
            "Can't blit from attachment %i, FBO only has %i attachments!",
            srcAttachmentIdx,
            m_spec.colorAttachments.size());
    }

    if (destAttachmentIdx >= destination.getSpec().colorAttachments.size())
    {
        Log::coreError(
            "Can't blit from attachment %i, FBO only has %i attachments!",
            destAttachmentIdx,
            destination.getSpec().colorAttachments.size());
    }

    uint32_t width1  = m_spec.width;
    uint32_t height1 = m_spec.height;
    uint32_t width2  = destination.getSpec().width;
    uint32_t height2 = destination.getSpec().height;

    Renderer::s_submit(
        [id1,
         id2,
         width1,
         height1,
         width2,
         height2,
         srcAttachmentIdx,
         destAttachmentIdx]()
        {
            glNamedFramebufferReadBuffer(
                id1, GL_COLOR_ATTACHMENT0 + srcAttachmentIdx);

            GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0 + destAttachmentIdx};

            glNamedFramebufferDrawBuffers(id2, 1, drawBuffers);

            glBlitNamedFramebuffer(id1,
                                   id2,
                                   0,
                                   0,
                                   width1,
                                   height1,
                                   0,
                                   0,
                                   width2,
                                   height2,
                                   GL_COLOR_BUFFER_BIT,
                                   GL_NEAREST);
        });
}

void GlFrameBuffer::bind(Mode mode) const noexcept
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

void GlFrameBuffer::unbind(Mode mode) const noexcept
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

void GlFrameBuffer::bindTexture(const uint32_t textureUnit,
                                const uint32_t attachmentIdx) const noexcept
{
    if (attachmentIdx >= m_textures.size())
    {
        return;
    }

    m_textures[attachmentIdx]->bind(textureUnit);
}

void GlFrameBuffer::unbindTexture(const uint32_t attachmentIdx) const noexcept
{
    if (attachmentIdx >= m_textures.size())
    {
        return;
    }

    m_textures[attachmentIdx]->unbind();
}

void GlFrameBuffer::clearColorAttachment(
    const uint32_t attachmentIdx) noexcept
{
    NM_PROFILE_DETAIL();

    if (attachmentIdx >= m_textures.size())
    {
        return;
    }

    ref<GlFrameBuffer> p_this = this;

    switch (m_textures[attachmentIdx]->getSpec().formatInternal)
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
                    GLfloat clearValue[4] = {0.0f, 0.0f, 0.0f, 0.0f};
                    glClearNamedFramebufferfv(
                        p_this->m_fbo, GL_COLOR, attachmentIdx, clearValue);
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
                    GLint clearValue[4] = {0, 0, 0, 0};
                    glClearNamedFramebufferiv(
                        p_this->m_fbo, GL_COLOR, attachmentIdx, clearValue);
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
                    GLuint clearValue[4] = {0, 0, 0, 0};
                    glClearNamedFramebufferuiv(
                        p_this->m_fbo, GL_COLOR, attachmentIdx, clearValue);
                });
            break;
        }
         default:
            NM_CORE_ASSERT(false,
                           "Can't clear framebuffer attachment type %i",
                           m_textures[attachmentIdx]->getSpec().formatInternal);
            break;
    }
}

void GlFrameBuffer::clearDepthAttachment() noexcept
{
    ref<GlFrameBuffer> p_this = this;

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

void GlFrameBuffer::clearAllAttachments() noexcept
{
    for (uint32_t i = 0; i < m_spec.colorAttachments.size(); i++)
    {
         clearColorAttachment(i);
    }
    if (m_spec.depthType != Texture::FormatInternal::NONE)
    {
         clearDepthAttachment();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////
void GlFrameBuffer::_construct() noexcept
{
    NM_PROFILE();

    ////////////////////////////////////////////////////////////////////////////
    // Remove these outside lamba otherwise these would not be delete
    // until new buffer textures are created because deleting these
    // is a render call
    ///////////////////////////////////////////////////////////////////////////
    if (m_fbo)
    {
        m_textures.clear();
    }

    ref<GlFrameBuffer> p_this = this;
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

                p_this->m_textures.push_back(texture);

                glNamedFramebufferTexture(p_this->m_fbo,
                                          GL_COLOR_ATTACHMENT0 + i,
                                          texture->getId(),
                                          0);

                colorAttachments[i] = GL_COLOR_ATTACHMENT0 + i;
            }

            ////////////////////////////////////////////////////////////////////
            // Tell GL To draw to all of our color attachments
            ////////////////////////////////////////////////////////////////////
            glNamedFramebufferDrawBuffers(
                p_this->m_fbo,
                p_this->m_spec.colorAttachments.size(),
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

uint32_t GlFrameBuffer::_textureTarget() const noexcept
{
    return m_spec.samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

}  // namespace nimbus