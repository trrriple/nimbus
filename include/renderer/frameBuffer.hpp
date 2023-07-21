#pragma once
#include "common.hpp"

namespace nimbus
{

class FrameBuffer
{
 
    // does anyone even have 8k res?
    inline static const uint32_t k_maxDimension = 8192;

   public:
    FrameBuffer(uint32_t width, uint32_t height);
    ~FrameBuffer();

    void construct();

    void resize(uint32_t width, uint32_t height);

    void bind() const;

    void unbind() const;
    
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

    uint32_t m_fbo;
    uint32_t m_texture;
    uint32_t m_rbo;


};

}  // namespace nimbus