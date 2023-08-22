////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Buffer abstrations, highly influenced by the implementation in
// Hazel. Credit where credit is due!
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "nimbus/renderer/buffer.hpp"

#include "nimbus/core/common.hpp"

#include <cstdint>
#include <vector>

namespace nimbus
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex Buffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class GlVertexBuffer : public VertexBuffer
{
   public:
    GlVertexBuffer(const void* vertices, u32_t size, VertexBuffer::Type type = VertexBuffer::Type::staticDraw);

    virtual ~GlVertexBuffer();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual void setData(const void* data, u32_t size) override;

    inline virtual const BufferFormat& getFormat() const override
    {
        return m_format;
    }

    inline virtual void setFormat(const BufferFormat& format) override
    {
        m_format = format;
    }

   private:
    bool m_mapped = false;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Index Buffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class GlIndexBuffer : public IndexBuffer
{
   public:
    GlIndexBuffer(u32_t* indices, u32_t count);
    GlIndexBuffer(u16_t* indices, u32_t count);
    GlIndexBuffer(u8_t* indices, u32_t count);

    virtual ~GlIndexBuffer();

    virtual void bind() const override;

    virtual void unbind() const override;

    inline u32_t getCount() const override
    {
        return m_count;
    }

    inline u32_t getType() const override
    {
        return m_type;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex Array
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class GlVertexArray : public VertexArray
{
   public:
    GlVertexArray();
    ~GlVertexArray();

    virtual void bind() const override;

    virtual void unbind() const override;

    virtual void addVertexBuffer(ref<VertexBuffer> p_vertexBuffer) override;

    virtual void setIndexBuffer(ref<IndexBuffer> p_indexBuffer) override;

    inline virtual const std::vector<ref<VertexBuffer>>& getVertexBuffers() const override
    {
        return m_vertexBuffers;
    }

    inline virtual const ref<IndexBuffer>& getIndexBuffer() const override
    {
        return m_indexBuffer;
    }

    inline virtual u32_t getExpectedVertexCount() override
    {
        return m_expectedVboVertexCount;
    }
};

}  // namespace nimbus
