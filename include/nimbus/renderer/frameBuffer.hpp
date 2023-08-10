#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/renderer/texture.hpp"

namespace nimbus
{

class FrameBuffer : public refCounted
{
   public:
    // does anyone even have 8k res?
    inline static const uint32_t k_maxDimension = 8192;

    struct Spec
    {
        uint32_t width   = 1280;
        uint32_t height  = 720;
        uint32_t samples = 1;

        std::vector<Texture::Spec> colorAttachments;
        Texture::FormatInternal    depthType
            = Texture::FormatInternal::DEPTH24_STENCIL8;
    };

    enum class Mode
    {
        READ_WRITE,
        READ,
        WRITE,
    };

    static ref<FrameBuffer> s_create(FrameBuffer::Spec& spec) noexcept;

    virtual ~FrameBuffer() = default;

    virtual void resize(uint32_t width, uint32_t height) = 0;

    virtual void blit(const FrameBuffer& destination) const = 0;

    virtual void bind(Mode mode = Mode::READ_WRITE) const = 0;

    virtual void unbind(Mode mode = Mode::READ_WRITE) const = 0;

    virtual void bindTexture(const uint32_t textureUnit,
                             const uint32_t attachmentIdx = 0) const
        = 0;

    virtual void unbindTexture(const uint32_t attachmentIdx = 0) const = 0;

    virtual void clear(const uint32_t attachmentIdx = 0) const = 0;

    virtual uint32_t getId() const = 0;

    virtual const Spec& getSpec() const = 0;

    virtual uint32_t getTextureId(const uint32_t attachmentIdx = 0) const = 0;

   protected:
    Spec                      m_spec;
    uint32_t                  m_fbo = 0;
    std::vector<ref<Texture>> m_textures;  // for texture (color) attachments
    uint32_t                  m_rbo = 0;   // for depth/stencil texture
};

}  // namespace nimbus