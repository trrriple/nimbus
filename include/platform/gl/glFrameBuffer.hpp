#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/renderer/frameBuffer.hpp"

namespace nimbus
{

class GlFrameBuffer : public FrameBuffer
{
   public:
    GlFrameBuffer(Spec& spec) noexcept;
    virtual ~GlFrameBuffer() noexcept override;

    virtual void resize(uint32_t width, uint32_t height) noexcept override;

    virtual void blit(const FrameBuffer& destination) const noexcept override;

    virtual void bind(Mode mode = Mode::READ_WRITE) const noexcept override;

    virtual void unbind(Mode mode = Mode::READ_WRITE) const noexcept override;

    virtual void bindTexture(const uint32_t textureUnit,
                             const uint32_t attachmentIdx
                             = 0) const noexcept override;

    virtual void unbindTexture(
        const uint32_t attachmentIdx) const noexcept override;

    virtual void clear(const uint32_t attachmentIdx
                       = 0) const noexcept override;

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
        if (attachmentIdx >= m_textures.size())
        {
            Log::coreError(
                "Attachment Index %i out of range. FrameBuffer has %i "
                "attachements",
                attachmentIdx,
                m_textures.size());

            return 0;
        }

        return m_textures[attachmentIdx]->getId();
    }

   private:
    uint32_t _textureTarget() const noexcept;

    void _construct() noexcept;
};

}  // namespace nimbus