#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/renderer/texture.hpp"

#include <mutex>
#include <variant>

namespace nimbus
{

class Framebuffer : public refCounted
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
        Texture::FormatInternal    depthType = Texture::FormatInternal::DEPTH24_STENCIL8;
    };

    enum class Mode
    {
        READ_WRITE,
        READ,
        WRITE,
    };

    struct PixelValues
    {
        bool                                              valid = false;
        std::variant<uint32_t, int32_t, float, glm::vec4> value;
    };

    class PixelReadRequest : public refCounted
    {
       public:
        PixelReadRequest(uint32_t iAttachmentIdx, uint32_t iX, uint32_t iY)
            : attachmentIdx(iAttachmentIdx), x(iX), y(iY)
        {
        }

        PixelReadRequest()
        {
        }

        void updateRequest(uint32_t iAttachmentIdx, uint32_t iX, uint32_t iY)
        {
            std::lock_guard<std::mutex> lock(mtx);
            attachmentIdx = iAttachmentIdx;
            x             = iX;
            y             = iY;
        }

        PixelValues getValue()
        {
            std::lock_guard<std::mutex> lock(mtx);
            return pixValue;
        }

        void invalidiate()
        {
            std::lock_guard<std::mutex> lock(mtx);
            pixValue.valid = false;
        }

        PixelValues getAndInvalidateValue()
        {
            std::lock_guard<std::mutex> lock(mtx);

            PixelValues unmodified = pixValue;
            if (pixValue.valid)
            {
                pixValue.valid = false;
            }

            return unmodified;
        }

        std::tuple<uint32_t, uint32_t, uint32_t> getLocation()
        {
            std::lock_guard<std::mutex> lock(mtx);

            return {attachmentIdx, x, y};
        }

        template <typename T>
        void setValue(const T& value)
        {
            std::lock_guard<std::mutex> lock(mtx);
            pixValue.value = value;
            pixValue.valid = true;
        }

       private:
        std::mutex  mtx;
        uint32_t    attachmentIdx;
        uint32_t    x;
        uint32_t    y;
        PixelValues pixValue;
    };

    static ref<Framebuffer> s_create(Framebuffer::Spec& spec);

    virtual ~Framebuffer() = default;

    virtual void resize(uint32_t width, uint32_t height) = 0;

    virtual void blit(ref<Framebuffer> p_destination,
                      const uint32_t   srcAttachmentIdx  = 0,
                      const uint32_t   destAttachmentIdx = 0) const
        = 0;

    virtual void bind(Mode mode = Mode::READ_WRITE) const = 0;

    virtual void unbind(Mode mode = Mode::READ_WRITE) const = 0;

    virtual void bindTexture(const uint32_t textureUnit, const uint32_t attachmentIdx = 0) const = 0;

    virtual void unbindTexture(const uint32_t attachmentIdx = 0) const = 0;

    virtual void clearColorAttachment(const uint32_t attachmentIdx = 0) = 0;

    virtual void clearDepthAttachment() = 0;

    virtual void clearAllAttachments() = 0;

    virtual uint32_t getId() const = 0;

    virtual const Spec& getSpec() const = 0;

    virtual uint32_t getTextureId(const uint32_t attachmentIdx = 0) const = 0;

    virtual void requestPixel(ref<PixelReadRequest> p_request) = 0;

   protected:
    Spec                      m_spec;
    uint32_t                  m_fbo = 0;
    std::vector<ref<Texture>> m_colorAttachments;  // for texture (color) attachments
    uint32_t                  m_rbo = 0;           // for depth/stencil texture
};

}  // namespace nimbus