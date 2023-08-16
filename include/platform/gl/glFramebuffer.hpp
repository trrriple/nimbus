#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/renderer/framebuffer.hpp"

#include <mutex>

namespace nimbus
{

class GlFramebuffer : public Framebuffer
{
   public:
    GlFramebuffer(Spec& spec);
    virtual ~GlFramebuffer() override;

    virtual void resize(uint32_t width, uint32_t height) override;

    virtual void blit(ref<Framebuffer> p_destination,
                      const uint32_t   srcAttachmentIdx  = 0,
                      const uint32_t   destAttachmentIdx = 0) const override;

    virtual void bind(Mode mode = Mode::READ_WRITE) const override;

    virtual void unbind(Mode mode = Mode::READ_WRITE) const override;

    virtual void bindTexture(const uint32_t textureUnit,
                             const uint32_t attachmentIdx = 0) const override;

    virtual void unbindTexture(const uint32_t attachmentIdx) const override;

    virtual void clearColorAttachment(const uint32_t attachmentIdx
                                      = 0) override;

    virtual void clearDepthAttachment() override;

    virtual void clearAllAttachments() override;

    inline virtual uint32_t getId() const override
    {
        return m_fbo;
    }

    inline virtual const Spec& getSpec() const override
    {
        return m_spec;
    }

    virtual uint32_t getTextureId(const uint32_t attachmentIdx
                                  = 0) const override
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

    virtual void requestPixel(ref<PixelReadRequest> p_request) override;

   private:
    uint32_t _textureTarget() const;

    void _construct();
};

}  // namespace nimbus