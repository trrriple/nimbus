////////////////////////////////////////////////////////////////////////////////
// Buffer abstrations, highly influenced by the implementation in
// Hazel. Credit where credit is due!
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "nimbus/renderer/buffer.hpp"

#include "nimbus/common.hpp"

#include <cstdint>
#include <vector>

namespace nimbus
{

////////////////////////////////////////////////////////////////////////////////
// Vertex Buffer
////////////////////////////////////////////////////////////////////////////////
class GlVertexBuffer : public VertexBuffer
{
   public:
    enum class Type
    {
        STATIC_DRAW,
        DYNAMIC_DRAW,
        STREAM_DRAW,
    };

    GlVertexBuffer(const void*        vertices,
                   uint32_t           size,
                   VertexBuffer::Type type = VertexBuffer::Type::STATIC_DRAW);

    virtual ~GlVertexBuffer();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual void setData(const void* data, uint32_t size) override;

    virtual const BufferFormat& getFormat() const override
    {
        return m_format;
    }

    virtual void setFormat(const BufferFormat& format) override
    {
        m_format = format;
    }

    virtual uint32_t getSize() override
    {
        return m_size;
    }

    virtual uint32_t getId() override
    {
        return m_id;
    }

};

////////////////////////////////////////////////////////////////////////////////
// Index Buffer
////////////////////////////////////////////////////////////////////////////////
class GlIndexBuffer : public IndexBuffer
{
   public:
    GlIndexBuffer(uint32_t* indices, uint32_t count);
    GlIndexBuffer(uint16_t* indices, uint32_t count);
    GlIndexBuffer(uint8_t* indices, uint32_t count);

    virtual ~GlIndexBuffer();

    virtual void bind() const override;

    virtual void unbind() const override;

    uint32_t getCount() const override
    {
        return m_count;
    }

    uint32_t getType() const override
    {
        return m_type;
    }


};

////////////////////////////////////////////////////////////////////////////////
// Vertex Array
////////////////////////////////////////////////////////////////////////////////
class GlVertexArray : public VertexArray
{
   public:
    GlVertexArray();
    ~GlVertexArray();

    virtual void bind() const override;

    virtual void unbind() const override;

    virtual void addVertexBuffer(
        const ref<VertexBuffer>& p_vertexBuffer) override;

    virtual void setIndexBuffer(const ref<IndexBuffer>& p_indexBuffer) override;

    virtual const std::vector<ref<VertexBuffer>>& getVertexBuffers()
        const override
    {
        return m_vertexBuffers;
    }

    virtual const ref<IndexBuffer>& getIndexBuffer() const override
    {
        return m_indexBuffer;
    }

    virtual uint32_t getExpectedVertexCount() override
    {
        return m_expectedVboVertexCount;
    }

};

}  // namespace nimbus
