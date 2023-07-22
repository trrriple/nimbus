
#include "renderer/frameBuffer.hpp"

#include "core.hpp"
#include "nmpch.hpp"
#include "renderer/texture.hpp"

namespace nimbus
{

////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////
FrameBuffer::FrameBuffer(uint32_t width, uint32_t height, uint32_t samples)
    : m_width(width), m_height(height), m_samples(samples)
{
    if (width == 0 || height == 0 || width > FrameBuffer::k_maxDimension
        || height > FrameBuffer::k_maxDimension)
    {
        NM_CORE_ASSERT(
            false,
            "Attempted to make null sized FrameBuffer or Yuge (> %i) "
            "FrameBuffer\n",
            FrameBuffer::k_maxDimension);
    }

    NM_CORE_ASSERT(m_samples, "Must have at least 1 sample ");

    if (m_samples > 1)
    {
        int32_t maxSamples;
        glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);

        NM_CORE_ASSERT(((int32_t)samples <= maxSamples),
                       "Sample count %i exceeds max supported samples %i",
                       m_samples,
                       maxSamples);
    }

    construct();
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteTextures(1, &m_texture);
    glDeleteRenderbuffers(1, &m_rbo);
}

void FrameBuffer::construct()
{
    ////////////////////////////////////////////////////////////////////////////
    // blow away existing frame buffer if we've already made it and create new
    ////////////////////////////////////////////////////////////////////////////
    if (m_fbo)
    {
        glDeleteFramebuffers(1, &m_fbo);
        glDeleteTextures(1, &m_texture);
        glDeleteRenderbuffers(1, &m_rbo);
    }

    glCreateFramebuffers(1, &m_fbo);

    ////////////////////////////////////////////////////////////////////////////
    // generate and bind the texture in which to write
    ////////////////////////////////////////////////////////////////////////////
    Texture::s_gen(m_texture, m_samples > 1);
    glBindTexture(_textureType(), m_texture);

    
    ////////////////////////////////////////////////////////////////////////////
    // setup texture and parameters
    ////////////////////////////////////////////////////////////////////////////
    if (m_samples == 1)
    {
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_width, m_height);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else
    {
        glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,
                                  m_samples,
                                  GL_RGBA8,
                                  m_width,
                                  m_height,
                                  GL_TRUE);
    }

    glBindTexture(_textureType(), 0);
    glNamedFramebufferTexture(m_fbo, GL_COLOR_ATTACHMENT0, m_texture, 0);

    ////////////////////////////////////////////////////////////////////////////
    // use render buffer for depth and stencil
    ////////////////////////////////////////////////////////////////////////////
    glCreateRenderbuffers(1, &m_rbo);
    // allocate
    if (m_samples == 1)
    {
        glNamedRenderbufferStorage(
            m_rbo, GL_DEPTH24_STENCIL8, m_width, m_height);
    }
    else
    {
        glNamedRenderbufferStorageMultisample(
            m_rbo, m_samples, GL_DEPTH24_STENCIL8, m_width, m_height);
    }

    glNamedFramebufferRenderbuffer(
        m_fbo, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);


    ////////////////////////////////////////////////////////////////////////////
    // Verify complete frame buffer
    ////////////////////////////////////////////////////////////////////////////
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        NM_CORE_ASSERT(false,
                       "Incomplete framebuffer! Error 0x%X\n",
                       glCheckFramebufferStatus(GL_FRAMEBUFFER));

        NM_CORE_CRITICAL("Incomplete framebuffer! Error 0x%X\n",
                         glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }

    // bind the default frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::resize(uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0 || width > FrameBuffer::k_maxDimension
        || height > FrameBuffer::k_maxDimension)
    {
        NM_CORE_ASSERT(
            false,
            "Attempted to make null sized FrameBuffer or Yuge (> %i) "
            "FrameBuffer\n",
            FrameBuffer::k_maxDimension);
    }

    m_width  = width;
    m_height = height;

    construct();
}

void FrameBuffer::blit(const FrameBuffer& destination) const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destination.m_fbo);

    glBlitFramebuffer(0,
                      0,
                      m_width,
                      m_height,
                      0,
                      0,
                      destination.m_width,
                      destination.m_height,
                      GL_COLOR_BUFFER_BIT,
                      GL_NEAREST);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void FrameBuffer::bind(Mode mode) const
{
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

    glViewport(0, 0, m_width, m_height);
}

void FrameBuffer::unbind(Mode mode) const
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
}

void FrameBuffer::bindTexture(const uint32_t textureUnit) const
{
    Texture::s_bind(m_texture, textureUnit, m_samples > 1);
}

void FrameBuffer::unbindTexture() const
{
    Texture::s_unbind(m_samples > 1);
}

void FrameBuffer::clear()
{
    int val = -1;
    glClearTexImage(m_texture, 0, GL_RGBA, GL_INT, &val);
}

////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////
uint32_t FrameBuffer::_textureType() const
{
    return m_samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

}  // namespace nimbus