#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/renderer/buffer.hpp"

#include "platform/gl/glBuffer.hpp"

namespace nimbus
{
////////////////////////////////////////////////////////////////////////////////
// Vertex Buffer
////////////////////////////////////////////////////////////////////////////////
ref<VertexBuffer> VertexBuffer::s_create(const void*        vertices,
                                         uint32_t           size,
                                         VertexBuffer::Type type)

{
    return ref<GlVertexBuffer>::gen(vertices, size, type);
}

////////////////////////////////////////////////////////////////////////////////
// Index Buffer
////////////////////////////////////////////////////////////////////////////////
ref<IndexBuffer> IndexBuffer::s_create(uint32_t* indices, uint32_t count)
{
    return ref<GlIndexBuffer>::gen(indices, count);
}

ref<IndexBuffer> IndexBuffer::s_create(uint16_t* indices, uint32_t count)
{
    return ref<GlIndexBuffer>::gen(indices, count);
}

ref<IndexBuffer> IndexBuffer::s_create(uint8_t* indices, uint32_t count)
{
    return ref<GlIndexBuffer>::gen(indices, count);
}

////////////////////////////////////////////////////////////////////////////////
// Vertex Array
////////////////////////////////////////////////////////////////////////////////
ref<VertexArray> VertexArray::s_create()
{
    return ref<GlVertexArray>::gen();
}

};  // namespace nimbus