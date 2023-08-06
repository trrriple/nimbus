
#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "platform/gl/glFrameBuffer.hpp"

#include "nimbus/renderer/texture.hpp"
#include "nimbus/renderer/graphicsApi.hpp"

#include "glad.h"

namespace nimbus
{

////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////
GlFrameBuffer::GlFrameBuffer(FrameBuffer::Spec& spec)
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

    if (spec.samples > 1)
    {
        int32_t maxSamples;
        glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);

        NM_CORE_ASSERT(((int32_t)spec.samples <= maxSamples),
                       "Sample count %i exceeds max supported samples %i",
                       spec.samples,
                       maxSamples);
    }

    _construct();
}

GlFrameBuffer::~GlFrameBuffer()
{
    NM_PROFILE_DETAIL();

    glDeleteFramebuffers(1, &m_fbo);
    m_textures.clear();
    glDeleteRenderbuffers(1, &m_rbo);
}

void GlFrameBuffer::resize(uint32_t width, uint32_t height)
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

void GlFrameBuffer::blit(const FrameBuffer& destination) const
{
    NM_PROFILE();

    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destination.getId());

    glBlitFramebuffer(0,
                      0,
                      m_spec.width,
                      m_spec.height,
                      0,
                      0,
                      destination.getSpec().width,
                      destination.getSpec().height,
                      GL_COLOR_BUFFER_BIT,
                      GL_NEAREST);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void GlFrameBuffer::bind(Mode mode) const
{
    NM_PROFILE_TRACE();

    switch (mode)
    {
        case (Mode::READ_WRITE):
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
            break;
        }
        case (Mode::READ):
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
            break;
        }
        case (Mode::WRITE):
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
            break;
        }
    }

    glViewport(0, 0, m_spec.width, m_spec.height);
}

void GlFrameBuffer::unbind(Mode mode) const
{
    NM_PROFILE_TRACE();

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
}

void GlFrameBuffer::bindTexture(const uint32_t textureUnit,
                                const uint32_t attachmentIdx) const
{
    m_textures[attachmentIdx]->bind(textureUnit);
}

void GlFrameBuffer::unbindTexture(const uint32_t attachmentIdx) const
{
    m_textures[attachmentIdx]->unbind();
}

void GlFrameBuffer::clear(const uint32_t attachmentIdx) const
{
    NM_PROFILE_DETAIL();

    int val = -1;
    glClearTexImage(
        m_textures[attachmentIdx]->getId(), 0, GL_RGBA, GL_INT, &val);
}

////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////
void GlFrameBuffer::_construct()
{
    NM_PROFILE();

    ////////////////////////////////////////////////////////////////////////////
    // blow away existing frame buffer if we've already made it and create new
    ////////////////////////////////////////////////////////////////////////////
    if (m_fbo)
    {
        glDeleteFramebuffers(1, &m_fbo);
        m_textures.clear();
        glDeleteRenderbuffers(1, &m_rbo);
    }

    glCreateFramebuffers(1, &m_fbo);

    for (auto texSpec : m_spec.colorAttachments)
    {
        ////////////////////////////////////////////////////////////////////////
        // generate and bind the texture in which to write
        ////////////////////////////////////////////////////////////////////////
        if (texSpec.samples != m_spec.samples)
        {
            NM_CORE_ASSERT(
                false,
                "Color attachment texture spec samples must match framebuffer "
                "spec samples!");
        }

        if (texSpec.width != m_spec.width || texSpec.height != m_spec.height)
        {
            NM_CORE_ASSERT(
                false,
                "Color attachment texture dimensions must match framebuffer "
                "dimensions!");
        }

        ref<Texture> texture
            = Texture::s_create(Texture::Type::DIFFUSE, texSpec);

        m_textures.push_back(texture);

        glNamedFramebufferTexture(
            m_fbo, GL_COLOR_ATTACHMENT0, texture->getId(), 0);
    }

    ////////////////////////////////////////////////////////////////////////////
    // use render buffer for depth and stencil
    ////////////////////////////////////////////////////////////////////////////

    if (Texture::s_formatInternal(m_spec.depthType))
    {
        glCreateRenderbuffers(1, &m_rbo);
        // allocate
        if (m_spec.samples == 1)
        {
            glNamedRenderbufferStorage(
                m_rbo,
                Texture::s_formatInternal(m_spec.depthType),
                m_spec.width,
                m_spec.height);
        }
        else
        {
            glNamedRenderbufferStorageMultisample(
                m_rbo,
                m_spec.samples,
                Texture::s_formatInternal(m_spec.depthType),
                m_spec.width,
                m_spec.height);
        }

        glNamedFramebufferRenderbuffer(
            m_fbo, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
    }

    ////////////////////////////////////////////////////////////////////////////
    // Verify complete frame buffer
    ////////////////////////////////////////////////////////////////////////////
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        NM_CORE_ASSERT(false,
                       "Incomplete framebuffer! Error 0x%X",
                       glCheckFramebufferStatus(GL_FRAMEBUFFER));

        Log::coreCritical("Incomplete framebuffer! Error 0x%X",
                          glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    // bind the default frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint32_t GlFrameBuffer::_textureTarget() const
{
    return m_spec.samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

}  // namespace nimbus