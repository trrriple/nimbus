#pragma once
#include "common.hpp"

namespace nimbus
{

class FrameBuffer
{
   public:
    // does anyone even have 8k res?
    inline static const uint32_t k_maxDimension = 8192;

    // TODO: should this be in texture class?
    enum class TextureFormat
    {
        RGBA8
    };

    enum class TextureFilterType
    {
        LINEAR
    };

    enum class TextureWrapType
    {
        CLAMP_TO_EDGE,
        REPEAT
    };

    enum class TextureDepthType
    {
        NONE,
        DEPTH24STENCIL8
    };

    struct TextureSpec
    {
        TextureFormat     type          = TextureFormat::RGBA8;
        TextureFilterType filterTypeMin = TextureFilterType::LINEAR;
        TextureFilterType filterTypeMag = TextureFilterType::LINEAR;
        TextureWrapType   wrapTypeR     = TextureWrapType::CLAMP_TO_EDGE;
        TextureWrapType   wrapTypeS     = TextureWrapType::CLAMP_TO_EDGE;
        TextureWrapType   wrapTypeT     = TextureWrapType::CLAMP_TO_EDGE;
    };

    struct Spec
    {
        uint32_t width   = 1280;
        uint32_t height  = 720;
        uint32_t samples = 1;

        std::vector<TextureSpec> colorAttachments;
        TextureDepthType         depthType = TextureDepthType::DEPTH24STENCIL8;
    };

    enum class Mode
    {
        READ_WRITE,
        READ,
        WRITE,
    };

    FrameBuffer(Spec& spec);
    ~FrameBuffer();

    void construct();

    void resize(uint32_t width, uint32_t height);

    void blit(const FrameBuffer& destination) const;

    void bind(Mode mode = Mode::READ_WRITE) const;

    void unbind(Mode mode = Mode::READ_WRITE) const;

    void bindTexture(const uint32_t textureUnit,
                     const uint32_t attachmentIdx = 0) const;

    void unbindTexture() const;

    void clear(const uint32_t attachmentIdx = 0) const;

    uint32_t getTextureId(const uint32_t attachmentIdx = 0) const
    {
        return m_textures[attachmentIdx];
    }

   private:
    Spec m_spec;

    uint32_t              m_fbo = 0;
    std::vector<uint32_t> m_textures;  // for texture (color) attachments
    uint32_t              m_rbo = 0;   // for depth/stencil texture

    uint32_t _textureType() const;

    uint32_t _textureFormat(TextureFormat format) const;

    uint32_t _textureFilterType(TextureFilterType filterType) const;

    uint32_t _textureWrapType(TextureWrapType wrapType) const;

    uint32_t _textureDepthType(TextureDepthType depthType) const;
};

}  // namespace nimbus