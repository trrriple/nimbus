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
    return makeRef<GlVertexBuffer>(vertices, size, type);
}

////////////////////////////////////////////////////////////////////////////////
// Index Buffer
////////////////////////////////////////////////////////////////////////////////
ref<IndexBuffer> IndexBuffer::s_create(uint32_t* indices, uint32_t count)
{
    return makeRef<GlIndexBuffer>(indices, count);
}

ref<IndexBuffer> IndexBuffer::s_create(uint16_t* indices, uint32_t count)
{
    return makeRef<GlIndexBuffer>(indices, count);
}

ref<IndexBuffer> IndexBuffer::s_create(uint8_t* indices, uint32_t count)
{
    return makeRef<GlIndexBuffer>(indices, count);
}

////////////////////////////////////////////////////////////////////////////////
// Vertex Array
////////////////////////////////////////////////////////////////////////////////
ref<VertexArray> VertexArray::s_create()
{
    return makeRef<GlVertexArray>();
}

};  // namespace nimbus