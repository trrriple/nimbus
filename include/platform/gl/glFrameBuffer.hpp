#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/renderer/framebuffer.hpp"

#include <mutex>

namespace nimbus
{

class GlFramebuffer : public Framebuffer
{
   public:
    GlFramebuffer(Spec& spec) noexcept;
    virtual ~GlFramebuffer() noexcept override;

    virtual void resize(uint32_t width, uint32_t height) noexcept override;

    virtual void blit(ref<Framebuffer> p_destination,
                      const uint32_t   srcAttachmentIdx = 0,
                      const uint32_t   destAttachmentIdx
                      = 0) const noexcept override;

    virtual void bind(Mode mode = Mode::READ_WRITE) const noexcept override;

    virtual void unbind(Mode mode = Mode::READ_WRITE) const noexcept override;

    virtual void bindTexture(const uint32_t textureUnit,
                             const uint32_t attachmentIdx
                             = 0) const noexcept override;

    virtual void unbindTexture(
        const uint32_t attachmentIdx) const noexcept override;

    virtual void clearColorAttachment(const uint32_t attachmentIdx
                                      = 0) noexcept override;

    virtual void clearDepthAttachment() noexcept override;

    virtual void clearAllAttachments() noexcept override;

    inline virtual uint32_t getId() const noexcept override
    {
        return m_fbo;
    }

    inline virtual const Spec& getSpec() const noexcept override
    {
        return m_spec;
    }

    virtual uint32_t getTextureId(const uint32_t attachmentIdx
                                  = 0) const noexcept override
    {
        if (attachmentIdx >= m_colorAttachments.size())
        {
            Log::coreWarn("Color attachment Index out of range! (%i >= %i)",
                          attachmentIdx,
                          m_colorAttachments.size());

            return 0;
        }
        return m_colorAttachments[attachmentIdx]->getId();
    }

    virtual void requestPixel(
        ref<PixelReadRequest> p_request) noexcept override;

   private:
    uint32_t _textureTarget() const noexcept;

    void _construct() noexcept;
};

}  // namespace nimbus