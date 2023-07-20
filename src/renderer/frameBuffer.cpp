
#include "renderer/frameBuffer.hpp"

#include "core.hpp"
#include "nmpch.hpp"

namespace nimbus
{

FrameBuffer::FrameBuffer(uint32_t width, uint32_t height)
    : m_width(width), m_height(height)
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
    // blow away existing frame buffer if we've already made it
    if (m_fbo)
    {
        glDeleteFramebuffers(1, &m_fbo);
        glDeleteTextures(1, &m_texture);
        glDeleteRenderbuffers(1, &m_rbo);
    }

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // generate and bind the texture in which to write
    glCreateTextures(GL_TEXTURE_2D, 1, &m_texture);

    // setup texture and parameters
    glTextureStorage2D(m_texture, 1, GL_RGBA8, m_width, m_height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

    // use render buffer instead for depth and stencil
    glGenRenderbuffers(1, &m_rbo);
    // bind
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    // allocate
    glRenderbufferStorage(
        GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
    // now actually attach it
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
    // unbind
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // check frame buffer is complete
    NM_CORE_ASSERT(
        (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE),
        "Incomplete framebuffer! Error 0x%X\n",
        glCheckFramebufferStatus(GL_FRAMEBUFFER));

    // bind the default frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
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

void FrameBuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width, m_height);
}

void FrameBuffer::unbind()
{  
    // bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::clear()
{
    int val = -1;
    glClearTexImage(GL_FRAMEBUFFER, m_texture, GL_RGBA8, GL_INT, &val);
}

}  // namespace nimbus