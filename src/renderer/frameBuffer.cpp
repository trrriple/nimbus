
#include "renderer/frameBuffer.hpp"
#include "renderer/texture.hpp"

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

    glCreateFramebuffers(1, &m_fbo);

    // generate and bind the texture in which to write
    Texture::s_gen(m_texture);


    // setup texture and parameters
    glTextureStorage2D(m_texture, 1, GL_RGBA8, m_width, m_height);
    
    // glTexImage2D (mutable texture) needs bind
    // glBindTexture(GL_TEXTURE_2D, m_texture);
    // glTexImage2D(GL_TEXTURE_2D,
    //              0,
    //              GL_RGBA8,
    //              m_width,
    //              m_height,
    //              0,
    //              GL_RGBA,
    //              GL_UNSIGNED_BYTE,
    //              nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // attach it
    glNamedFramebufferTexture(m_fbo, GL_COLOR_ATTACHMENT0, m_texture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);


    // use render buffer instead for depth and stencil
    glCreateRenderbuffers(1, &m_rbo);
    // allocate
    glNamedRenderbufferStorage(m_rbo, GL_DEPTH24_STENCIL8, m_width, m_height);
    // now actually attach it
    glNamedFramebufferRenderbuffer(
        m_fbo, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

    // check frame buffer is complete
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

void FrameBuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width, m_height);
}

void FrameBuffer::unbind() const
{  
    // bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::bindTexture(const uint32_t textureUnit) const
{   
    Texture::s_bind(m_texture, textureUnit);
}

void FrameBuffer::unbindTexture() const
{
    Texture::s_unbind();
}

void FrameBuffer::clear()
{
    int val = -1;
    glClearTexImage(m_texture, 0, GL_RGBA, GL_INT, &val);
}

}  // namespace nimbus