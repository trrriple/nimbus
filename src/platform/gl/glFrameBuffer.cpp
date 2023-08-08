
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

    // TODO get from renderer(probably need to do the same for texturss)
    // if (spec.samples > 1)
    // {
    //     int32_t maxSamples;
    //     glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);

    //     NM_CORE_ASSERT(((int32_t)spec.samples <= maxSamples),
    //                    "Sample count %i exceeds max supported samples %i",
    //                    spec.samples,
    //                    maxSamples);
    // }

    _construct();
}

GlFrameBuffer::~GlFrameBuffer()
{
    NM_PROFILE_DETAIL();

    uint32_t id    = m_fbo;
    uint32_t rboId = m_rbo;

    Renderer::s_submit(
        [id, rboId]()
        {
            glDeleteFramebuffers(1, &id);
            glDeleteRenderbuffers(1, &rboId);
        });

    m_textures.clear();
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

    uint32_t id1     = m_fbo;
    uint32_t id2     = destination.getId();
    uint32_t width1  = m_spec.width;
    uint32_t height1 = m_spec.height;
    uint32_t width2  = destination.getSpec().width;
    uint32_t height2 = destination.getSpec().height;

    Renderer::s_submit(
        [id1, id2, width1, height1, width2, height2]()
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, id1);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id2);

            glBlitFramebuffer(0,
                              0,
                              width1,
                              height1,
                              0,
                              0,
                              width2,
                              height2,
                              GL_COLOR_BUFFER_BIT,
                              GL_NEAREST);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        });
}

void GlFrameBuffer::bind(Mode mode) const
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

void GlFrameBuffer::unbind(Mode mode) const
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
                                const uint32_t attachmentIdx) const
{
    if (attachmentIdx >= m_textures.size())
    {
        return;
    }

    m_textures[attachmentIdx]->bind(textureUnit);
}

void GlFrameBuffer::unbindTexture(const uint32_t attachmentIdx) const
{
    if (attachmentIdx >= m_textures.size())
    {
        return;
    }

    m_textures[attachmentIdx]->unbind();
}

void GlFrameBuffer::clear(const uint32_t attachmentIdx) const
{
    NM_PROFILE_DETAIL();

    if (attachmentIdx >= m_textures.size())
    {
        return;
    }

    uint32_t id = m_textures[attachmentIdx]->getId();
    Renderer::s_submit(
        [id]()
        {
            glClearTexImage(id, 0, GL_RGBA, GL_INT, nullptr);
        });
}

////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////
void GlFrameBuffer::_construct()
{
    NM_PROFILE();

    // ref<GlFrameBuffer> p_instance = makeRef<GlFrameBuffer>(*this);
    GlFrameBuffer* p_instance = this;


    ////////////////////////////////////////////////////////////////////////////
    // Remove these outside lamba otherwise these would not be delete
    // until new buffer textures are created because deleting these
    // is a render call
    ///////////////////////////////////////////////////////////////////////////
    if (p_instance->m_fbo)
    {
        p_instance->m_textures.clear();
    }

    Renderer::s_submit(
        [p_instance]()
        {
            ////////////////////////////////////////////////////////////////////
            // blow away existing frame buffer if we've already made it and
            // create new
            ////////////////////////////////////////////////////////////////////
            if (p_instance->m_fbo)
            {
                glDeleteFramebuffers(1, &p_instance->m_fbo);
                glDeleteRenderbuffers(1, &p_instance->m_rbo);
            }

            glCreateFramebuffers(1, &p_instance->m_fbo);

            for (auto texSpec : p_instance->m_spec.colorAttachments)
            {
                ////////////////////////////////////////////////////////////////
                // generate and bind the texture in which to write
                ////////////////////////////////////////////////////////////////
                if (texSpec.samples != p_instance->m_spec.samples)
                {
                    NM_CORE_ASSERT_STATIC(
                        false,
                        "Color attachment texture spec samples must "
                        "match framebuffer "
                        "spec samples!");
                }

                if (texSpec.width != p_instance->m_spec.width
                    || texSpec.height != p_instance->m_spec.height)
                {
                    NM_CORE_ASSERT_STATIC(
                        false,
                        "Color attachment texture dimensions must "
                        "match framebuffer "
                        "dimensions!");
                }

                ref<Texture> texture
                    = Texture::s_create(Texture::Type::DIFFUSE, texSpec, false);

                p_instance->m_textures.push_back(texture);

                glNamedFramebufferTexture(p_instance->m_fbo,
                                          GL_COLOR_ATTACHMENT0,
                                          texture->getId(),
                                          0);
            }

            ////////////////////////////////////////////////////////////////////
            // use render buffer for depth and stencil
            ////////////////////////////////////////////////////////////////////

            if (Texture::s_formatInternal(p_instance->m_spec.depthType))
            {
                glCreateRenderbuffers(1, &p_instance->m_rbo);
                // allocate
                if (p_instance->m_spec.samples == 1)
                {
                    glNamedRenderbufferStorage(
                        p_instance->m_rbo,
                        Texture::s_formatInternal(p_instance->m_spec.depthType),
                        p_instance->m_spec.width,
                        p_instance->m_spec.height);
                }
                else
                {
                    glNamedRenderbufferStorageMultisample(
                        p_instance->m_rbo,
                        p_instance->m_spec.samples,
                        Texture::s_formatInternal(p_instance->m_spec.depthType),
                        p_instance->m_spec.width,
                        p_instance->m_spec.height);
                }

                glNamedFramebufferRenderbuffer(p_instance->m_fbo,
                                               GL_DEPTH_STENCIL_ATTACHMENT,
                                               GL_RENDERBUFFER,
                                               p_instance->m_rbo);
            }

            ////////////////////////////////////////////////////////////////////////////
            // Verify complete frame buffer
            ////////////////////////////////////////////////////////////////////////////
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER)
                != GL_FRAMEBUFFER_COMPLETE)
            {
                NM_CORE_ASSERT_STATIC(false,
                               "Incomplete framebuffer! Error 0x%X",
                               glCheckFramebufferStatus(GL_FRAMEBUFFER));

                Log::coreCritical("Incomplete framebuffer! Error 0x%X",
                                  glCheckFramebufferStatus(GL_FRAMEBUFFER));
            }

            glBindFramebuffer(GL_FRAMEBUFFER, p_instance->m_fbo);
            // bind the default frame buffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

        });

}

uint32_t GlFrameBuffer::_textureTarget() const
{
    return m_spec.samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

}  // namespace nimbus