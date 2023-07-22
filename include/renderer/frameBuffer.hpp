#pragma once
#include "common.hpp"

namespace nimbus
{

class FrameBuffer
{
 
    // does anyone even have 8k res?
    inline static const uint32_t k_maxDimension = 8192;

   public:
    enum class Mode
    {
        READ_WRITE,
        READ,
        WRITE,
    };

    FrameBuffer(uint32_t width, uint32_t height, uint32_t samples = 1);
    ~FrameBuffer();

    void construct();

    void resize(uint32_t width, uint32_t height);

    void blit(const FrameBuffer& destination) const;

    void bind(Mode mode = Mode::READ_WRITE) const;

    void unbind(Mode mode = Mode::READ_WRITE) const;

    void bindTexture(const uint32_t textureUnit) const;

    void unbindTexture() const;

    void clear();


    uint32_t getTextureId() const
    {
        return m_texture;
    }

   private:
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_samples;

    uint32_t m_fbo     = 0;
    uint32_t m_texture = 0;
    uint32_t m_rbo     = 0;


    uint32_t _textureType() const;
};

}  // namespace nimbus