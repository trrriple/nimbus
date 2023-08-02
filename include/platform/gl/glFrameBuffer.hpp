#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/renderer/frameBuffer.hpp"

namespace nimbus
{

class GlFrameBuffer : public FrameBuffer
{
   public:
    GlFrameBuffer(Spec& spec);
    virtual ~GlFrameBuffer() override;

    virtual void resize(uint32_t width, uint32_t height) override;

    virtual void blit(const FrameBuffer& destination) const override;

    virtual void bind(Mode mode = Mode::READ_WRITE) const override;

    virtual void unbind(Mode mode = Mode::READ_WRITE) const override;

    virtual void bindTexture(const uint32_t textureUnit,
                             const uint32_t attachmentIdx = 0) const override;

    virtual void unbindTexture(const uint32_t attachmentIdx) const override;

    virtual void clear(const uint32_t attachmentIdx = 0) const override;

    virtual uint32_t getId() const override
    {
        return m_fbo;
    }

    virtual const Spec& getSpec() const override
    {
        return m_spec;
    }

    virtual uint32_t getTextureId(const uint32_t attachmentIdx
                                  = 0) const override
    {
        return m_textures[attachmentIdx]->getId();
    }

   private:
    uint32_t _textureTarget() const;

    void _construct();
};

}  // namespace nimbus