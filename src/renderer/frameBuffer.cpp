
#include "renderer/frameBuffer.hpp"

#include "core.hpp"
#include "nmpch.hpp"
#include "renderer/texture.hpp"

namespace nimbus
{

////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////
FrameBuffer::FrameBuffer(FrameBuffer::Spec& spec)
    : m_spec(spec)
{
    NM_PROFILE_DETAIL();

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

    construct();
}

FrameBuffer::~FrameBuffer()
{
    NM_PROFILE_DETAIL();

    glDeleteFramebuffers(1, &m_fbo);
    glDeleteTextures(m_textures.size(), m_textures.data());
    glDeleteRenderbuffers(1, &m_rbo);
}

void FrameBuffer::construct()
{
    NM_PROFILE();

    ////////////////////////////////////////////////////////////////////////////
    // blow away existing frame buffer if we've already made it and create new
    ////////////////////////////////////////////////////////////////////////////
    if (m_fbo)
    {
        glDeleteFramebuffers(1, &m_fbo);
        glDeleteTextures(m_textures.size(), m_textures.data());
        glDeleteRenderbuffers(1, &m_rbo);

        m_textures.clear();
    }

    glCreateFramebuffers(1, &m_fbo);

    for (auto texSpec : m_spec.colorAttachments)
    {
        ////////////////////////////////////////////////////////////////////////
        // generate and bind the texture in which to write
        ////////////////////////////////////////////////////////////////////////
        uint32_t texture;
        Texture::s_gen(texture, m_spec.samples > 1);

        m_textures.push_back(texture);

        glBindTexture(_textureTarget(), texture);

        ////////////////////////////////////////////////////////////////////////
        // setup texture and parameters
        ////////////////////////////////////////////////////////////////////////
        if (m_spec.samples == 1)
        {
            glTexStorage2D(GL_TEXTURE_2D,
                           1,
                           Texture::s_formatInternal(texSpec.formatInternal),
                           m_spec.width,
                           m_spec.height);

            ///////////////////////////
            // Texture parameters
            ///////////////////////////
            glTexParameteri(GL_TEXTURE_2D,
                            GL_TEXTURE_MAG_FILTER,
                            Texture::s_filterType(texSpec.filterTypeMag));

            glTexParameteri(GL_TEXTURE_2D,
                            GL_TEXTURE_MIN_FILTER,
                            Texture::s_filterType(texSpec.filterTypeMin));

            glTexParameteri(GL_TEXTURE_2D,
                            GL_TEXTURE_WRAP_R,
                            Texture::s_wrapType(texSpec.wrapTypeR));
            glTexParameteri(GL_TEXTURE_2D,
                            GL_TEXTURE_WRAP_S,
                            Texture::s_wrapType(texSpec.wrapTypeS));
            glTexParameteri(GL_TEXTURE_2D,
                            GL_TEXTURE_WRAP_T,
                             Texture::s_wrapType(texSpec.wrapTypeT));
        }
        else
        {
            glTexStorage2DMultisample(
                GL_TEXTURE_2D_MULTISAMPLE,
                m_spec.samples,
                Texture::s_formatInternal(texSpec.formatInternal),
                m_spec.width,
                m_spec.height,
                GL_TRUE);
        }

        glBindTexture(_textureTarget(), 0);
        glNamedFramebufferTexture(m_fbo, GL_COLOR_ATTACHMENT0, texture, 0);
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
                m_rbo, GL_DEPTH24_STENCIL8, m_spec.width, m_spec.height);
        }
        else
        {
            glNamedRenderbufferStorageMultisample(m_rbo,
                                                  m_spec.samples,
                                                  GL_DEPTH24_STENCIL8,
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

    // bind the default frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::resize(uint32_t width, uint32_t height)
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

    construct();
}

void FrameBuffer::blit(const FrameBuffer& destination) const
{
    NM_PROFILE();

    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destination.m_fbo);

    glBlitFramebuffer(0,
                      0,
                      m_spec.width,
                      m_spec.height,
                      0,
                      0,
                      destination.m_spec.width,
                      destination.m_spec.height,
                      GL_COLOR_BUFFER_BIT,
                      GL_NEAREST);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void FrameBuffer::bind(Mode mode) const
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

void FrameBuffer::unbind(Mode mode) const
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

void FrameBuffer::bindTexture(const uint32_t textureUnit,
                              const uint32_t attachmentIdx) const
{
    Texture::s_bind(m_textures[attachmentIdx], textureUnit, m_spec.samples > 1);
}

void FrameBuffer::unbindTexture() const
{
    Texture::s_unbind(m_spec.samples > 1);
}


void FrameBuffer::clear(const uint32_t attachmentIdx) const
{
    NM_PROFILE_DETAIL();

    int val = -1;
    glClearTexImage(m_textures[attachmentIdx], 0, GL_RGBA, GL_INT, &val);
}

////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////
uint32_t FrameBuffer::_textureTarget() const
{
    return m_spec.samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

}  // namespace nimbus