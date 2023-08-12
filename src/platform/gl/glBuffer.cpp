#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "platform/gl/glBuffer.hpp"
#include "nimbus/renderer/renderer.hpp"

#include "glad.h"

namespace nimbus
{
////////////////////////////////////////////////////////////////////////////////
// Vertex Buffer
////////////////////////////////////////////////////////////////////////////////
GlVertexBuffer::GlVertexBuffer(const void*        vertices,
                               uint32_t           size,
                               VertexBuffer::Type type) noexcept
{
    m_size = size;
    m_type = type;

    ref<GlVertexBuffer> p_this = this;

    void* localCpy = malloc(size);
    memcpy(localCpy, vertices, size);

    switch (m_type)
    {
        case (VertexBuffer::Type::STATIC_DRAW):
        {
            Renderer::s_submitObject(
                [p_this, localCpy]() mutable
                {
                    glCreateBuffers(1, &p_this->m_id);
                    glNamedBufferStorage(
                        p_this->m_id, p_this->m_size, localCpy, 0);

                    free(localCpy);
                });

            break;
        }
        case (VertexBuffer::Type::DYNAMIC_DRAW):
        case (VertexBuffer::Type::STREAM_DRAW):
        {
            Renderer::s_submitObject(
                [p_this, localCpy]() mutable
                {
                    glCreateBuffers(1, &p_this->m_id);

                    GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT
                                       | GL_MAP_COHERENT_BIT;
                    glNamedBufferStorage(
                        p_this->m_id, p_this->m_size, localCpy, flags);
                    p_this->mp_memory = glMapNamedBufferRange(
                        p_this->m_id, 0, p_this->m_size, flags);

                    p_this->m_mapped = true;

                    free(localCpy);
                });

            break;
        }
        default:
            NM_CORE_ASSERT(false, "Unknown VertexBuffer::Type %i", type);
            free(localCpy);
    }
}

GlVertexBuffer::~GlVertexBuffer() noexcept
{
    uint32_t id     = m_id;
    bool     mapped = m_mapped;
    Renderer::s_submitObject(
        [id, mapped]()
        {
            if (mapped)
            {
                glBindBuffer(GL_ARRAY_BUFFER, id);
                glUnmapBuffer(GL_ARRAY_BUFFER);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
            glDeleteBuffers(1, &id);
        });
}

void GlVertexBuffer::bind() const noexcept
{
    ref<GlVertexBuffer> p_this = const_cast<GlVertexBuffer*>(this);

    Renderer::s_submit([p_this]()
                       { glBindBuffer(GL_ARRAY_BUFFER, p_this->m_id); });
}

void GlVertexBuffer::unbind() const noexcept
{
    Renderer::s_submit([]() { glBindBuffer(GL_ARRAY_BUFFER, 0); });
}

void GlVertexBuffer::setData(const void* data, uint32_t size) noexcept
{
    NM_CORE_ASSERT(size <= m_size,
                   "Size (%i) must be <= preallocated size (%i)",
                   size,
                   m_size);

    NM_CORE_ASSERT(m_type != VertexBuffer::Type::STATIC_DRAW,
                   "Cannot set data in a static buffer!");

    if (m_mapped)
    {
        memcpy(mp_memory, data, size);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Index Buffer
////////////////////////////////////////////////////////////////////////////////
GlIndexBuffer::GlIndexBuffer(uint32_t* indices, uint32_t count) noexcept
{
    m_count = count;
    m_type  = GL_UNSIGNED_INT;

    ref<GlIndexBuffer> p_this = this;

    void* localCpy = malloc(p_this->m_count * sizeof(uint32_t));
    memcpy(localCpy, indices, p_this->m_count * sizeof(uint32_t));

    Renderer::s_submitObject(
        [p_this, localCpy]() mutable
        {
            glCreateBuffers(1, &p_this->m_id);
            glNamedBufferStorage(p_this->m_id,
                                 p_this->m_count * sizeof(uint32_t),
                                 localCpy,
                                 0);

            free(localCpy);
        });
}

GlIndexBuffer::GlIndexBuffer(uint16_t* indices, uint32_t count) noexcept
{
    m_count = count;
    m_type  = GL_UNSIGNED_SHORT;

    ref<GlIndexBuffer> p_this = this;

    void* localCpy = malloc(p_this->m_count * sizeof(uint16_t));
    memcpy(localCpy, indices, p_this->m_count * sizeof(uint16_t));

    Renderer::s_submitObject(
        [p_this, localCpy]() mutable
        {
            glCreateBuffers(1, &p_this->m_id);
            glNamedBufferStorage(p_this->m_id,
                                 p_this->m_count * sizeof(uint16_t),
                                 localCpy,
                                 0);

            free(localCpy);
        });
}

GlIndexBuffer::GlIndexBuffer(uint8_t* indices, uint32_t count) noexcept
{
    m_count = count;
    m_type  = GL_UNSIGNED_BYTE;

    ref<GlIndexBuffer> p_this = this;

    void* localCpy = malloc(p_this->m_count * sizeof(uint8_t));
    memcpy(localCpy, indices, p_this->m_count * sizeof(uint8_t));

    Renderer::s_submitObject(
        [p_this, localCpy]() mutable
        {
            glCreateBuffers(1, &p_this->m_id);
            glNamedBufferStorage(p_this->m_id,
                                 p_this->m_count * sizeof(uint8_t),
                                 localCpy,
                                 0);

            free(localCpy);
        });
}

GlIndexBuffer::~GlIndexBuffer() noexcept
{
    uint32_t id = m_id;
    Renderer::s_submitObject([id]() { glDeleteBuffers(1, &id); });
}

void GlIndexBuffer::bind() const noexcept
{
    ref<GlIndexBuffer> p_this = const_cast<GlIndexBuffer*>(this);

    Renderer::s_submit(
        [p_this]()
        { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p_this->m_id); });
}

void GlIndexBuffer::unbind() const noexcept
{
    Renderer::s_submit([]() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); });
}

////////////////////////////////////////////////////////////////////////////////
// Vertex Array
////////////////////////////////////////////////////////////////////////////////
GlVertexArray::GlVertexArray() noexcept
{
    ref<GlVertexArray> p_this = this;

        Renderer::s_submitObject(
            [p_this]() mutable
            {
                glCreateVertexArrays(1, &p_this->m_id);
            });
}

GlVertexArray::~GlVertexArray() noexcept
{
    uint32_t id = m_id;
    Renderer::s_submitObject([id]() { 
        glDeleteVertexArrays(1, &id); 
    });
}

void GlVertexArray::bind() const noexcept
{
    ref<GlVertexArray> p_this = const_cast<GlVertexArray*>(this);

    Renderer::s_submit(
        [p_this]()
        {
            glBindVertexArray(p_this->m_id);
        });
}

void GlVertexArray::unbind() const noexcept
{
    Renderer::s_submit([]() { glBindVertexArray(0); });
}

void GlVertexArray::addVertexBuffer(ref<VertexBuffer> p_vertexBuffer) noexcept
{
    NM_CORE_ASSERT(p_vertexBuffer->getFormat().getComponents().size(),
                   "VBO format is required to create VBA");

    ref<GlVertexArray> p_this = this;

    Renderer::s_submitObject(
        [p_this, p_vertexBuffer]() mutable
        {
            uint32_t vboId = p_vertexBuffer->getId();
            glBindVertexArray(p_this->m_id);
            glBindBuffer(GL_ARRAY_BUFFER, vboId);

            const auto& format = p_vertexBuffer->getFormat();
            for (const auto& component : format)
            {
                uint32_t glType
                    = Shader::s_getShaderType(std::get<0>(component.dataType));

                if (glType == GL_INT || glType == GL_UNSIGNED_INT
                    || glType == GL_BOOL)
                {
                    glEnableVertexAttribArray(p_this->m_vertexBufferIndex);

                    uint32_t numOfComponent = std::get<2>(component.dataType);

                    glVertexAttribIPointer(p_this->m_vertexBufferIndex,
                                           numOfComponent,
                                           glType,
                                           format.getStride(),
                                           (const void*)component.offset);

                    if (component.type == BufferComponent::Type::PER_INSTANCE)
                    {
                        glVertexAttribDivisor(p_this->m_vertexBufferIndex,
                                              component.perInstance);
                    }
                    p_this->m_vertexBufferIndex++;
                }
                else if (glType == GL_FLOAT)
                {
                    uint32_t numOfComponent = std::get<2>(component.dataType);
                    uint32_t columns        = 1;

                    uint32_t numOfComponentsPerColumn = numOfComponent;
                    if (numOfComponent > 4)
                    {
                        // This is a matrix we must add a pointer for each
                        // column of the matrix
                        columns
                            = static_cast<uint32_t>(std::sqrt(numOfComponent));
                        numOfComponentsPerColumn
                            = columns;  // matrix will always be square
                    }

                    for (uint32_t i = 0; i < columns; i++)
                    {
                        uint64_t offset
                            = component.offset
                              + (sizeof(float) * numOfComponentsPerColumn * i);

                        glEnableVertexAttribArray(
                            p_this->m_vertexBufferIndex);
                        glVertexAttribPointer(
                            p_this->m_vertexBufferIndex,
                            numOfComponentsPerColumn,
                            glType,
                            component.normalized ? GL_TRUE : GL_FALSE,
                            format.getStride(),
                            (const void*)offset);

                        if (component.type
                            == BufferComponent::Type::PER_INSTANCE)
                        {
                            glVertexAttribDivisor(
                                p_this->m_vertexBufferIndex,
                                component.perInstance);
                        }
                        p_this->m_vertexBufferIndex++;
                    }
                }
                else
                {
                    NM_CORE_ASSERT_STATIC(0, "Unknown ShaderDataType!");
                }
            }

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            uint32_t thisVboVertexCount
                = p_vertexBuffer->getSize() / format.getStride();

            // accumulate the size of the vertex
            p_this->m_vertexSize += format.getStride();

            if (p_this->m_vertexBuffers.size() == 0)
            {
                // this is our first vertex buffer, so set the expected size in
                // number of vertexes because this "should" to match between all
                // vbos bound to this vba
                p_this->m_expectedVboVertexCount = thisVboVertexCount;
            }

            p_this->m_vertexBuffers.push_back(p_vertexBuffer);
        });
}

void GlVertexArray::setIndexBuffer(ref<IndexBuffer> p_indexBuffer) noexcept
{
    ref<GlVertexArray> p_this = this;

    Renderer::s_submitObject(
        [p_this, p_indexBuffer]()
        {
            glBindVertexArray(p_this->m_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p_indexBuffer->getId());
            glBindVertexArray(0);
        });

    m_indexBuffer = p_indexBuffer;
}

};  // namespace nimbus