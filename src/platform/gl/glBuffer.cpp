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
                               VertexBuffer::Type type)
{
    m_size = size;
    m_type = type;

    // ref<GlVertexBuffer> p_instance = makeRef<GlVertexBuffer>(*this);
    GlVertexBuffer* p_instance = this;

    switch (m_type)
    {
        case (VertexBuffer::Type::STATIC_DRAW):
        {

            Renderer::s_submit(
                [p_instance, vertices]()
                {
                    glCreateBuffers(1, &p_instance->m_id);

                    glNamedBufferStorage(
                        p_instance->m_id, p_instance->m_size, vertices, 0);
                    p_instance->mp_memory = glMapNamedBufferRange(
                        p_instance->m_id, 0, p_instance->m_size, 0);

                    p_instance->m_mapped = true;
                });

                break;

        }
        case (VertexBuffer::Type::DYNAMIC_DRAW):
        case (VertexBuffer::Type::STREAM_DRAW):
        {

            Renderer::s_submit(
                [p_instance, vertices]()
                {   
                    glCreateBuffers(1, &p_instance->m_id);

                    GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT
                                       | GL_MAP_COHERENT_BIT;
                    glNamedBufferStorage(
                        p_instance->m_id, p_instance->m_size, vertices, flags);
                    p_instance->mp_memory = glMapNamedBufferRange(
                        p_instance->m_id, 0, p_instance->m_size, flags);

                    p_instance->m_mapped = true;
                });

            break;
        }
    }

}

GlVertexBuffer::~GlVertexBuffer()
{
    // ref<GlVertexBuffer> p_instance = makeRef<GlVertexBuffer>(*this);

    uint32_t id = m_id;
    Renderer::s_submit(
        [id]()
        {
            glBindBuffer(GL_ARRAY_BUFFER, id);
            glUnmapBuffer(GL_ARRAY_BUFFER);
            glDeleteBuffers(1, &id);
        });
}

void GlVertexBuffer::bind() const
{
    uint32_t id = m_id;
    Renderer::s_submit(
        [id]()
        {
            if (id != s_currBoundId)
            {
                glBindBuffer(GL_ARRAY_BUFFER, id);
                s_currBoundId = id;
            }
        });
}

void GlVertexBuffer::unbind() const
{
    Renderer::s_submit(
        []()
        {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        });
    s_currBoundId = 0;
}

void GlVertexBuffer::setData(const void* data, uint32_t size)
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
GlIndexBuffer::GlIndexBuffer(uint32_t* indices, uint32_t count)
{
    m_count = count;
    m_type  = GL_UNSIGNED_INT;
    
    // ref<GlIndexBuffer> p_instance       = makeRef<GlIndexBuffer>(*this);
    GlIndexBuffer* p_instance = this;

    void* localCpy = malloc(p_instance->m_count * sizeof(uint32_t));
    memcpy(localCpy, indices, p_instance->m_count * sizeof(uint32_t));

    Renderer::s_submit(
        [p_instance, localCpy]()
        {
            glCreateBuffers(1, &p_instance->m_id);
            glNamedBufferStorage(p_instance->m_id,
                                 p_instance->m_count * sizeof(uint32_t),
                                 localCpy,
                                 0);

            free(localCpy);
        });
}

GlIndexBuffer::GlIndexBuffer(uint16_t* indices, uint32_t count)
{
    m_count = count;
    m_type = GL_UNSIGNED_SHORT;

    // ref<GlIndexBuffer> p_instance       = makeRef<GlIndexBuffer>(*this);
    GlIndexBuffer* p_instance = this;

    void* localCpy = malloc(p_instance->m_count * sizeof(uint16_t));
    memcpy(localCpy, indices, p_instance->m_count * sizeof(uint16_t));

    Renderer::s_submit(
        [p_instance, localCpy]()
        {
            glCreateBuffers(1, &p_instance->m_id);
            glNamedBufferStorage(p_instance->m_id,
                                 p_instance->m_count * sizeof(uint16_t),
                                 localCpy,
                                 0);

            free(localCpy);

        });
}

GlIndexBuffer::GlIndexBuffer(uint8_t* indices, uint32_t count)
{
    m_count = count;
    m_type  = GL_UNSIGNED_BYTE;

    // ref<GlIndexBuffer> p_instance       = makeRef<GlIndexBuffer>(*this);
    GlIndexBuffer* p_instance = this;

    void* localCpy = malloc(p_instance->m_count * sizeof(uint8_t));
    memcpy(localCpy, indices, p_instance->m_count * sizeof(uint8_t));

    Renderer::s_submit(
        [p_instance, localCpy]()
        {
            glCreateBuffers(1, &p_instance->m_id);
            glNamedBufferStorage(p_instance->m_id,
                                 p_instance->m_count * sizeof(uint8_t),
                                 localCpy,
                                 0);

            free(localCpy);
        });
}

GlIndexBuffer::~GlIndexBuffer()
{
    uint32_t id = m_id;
    Renderer::s_submit([id]() { glDeleteBuffers(1, &id); });
}

void GlIndexBuffer::bind() const
{
    uint32_t id = m_id;
    Renderer::s_submit([id]() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); });
}

void GlIndexBuffer::unbind() const
{
    Renderer::s_submit([]() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); });
}

////////////////////////////////////////////////////////////////////////////////
// Vertex Array
////////////////////////////////////////////////////////////////////////////////
GlVertexArray::GlVertexArray()
{
    // ref<GlVertexArray> p_instance       = makeRef<GlVertexArray>(*this);
    GlVertexArray* p_instance = this;


    Renderer::s_submit(
        [p_instance]()
        {
            glCreateVertexArrays(1, &p_instance->m_id);

        });
}

GlVertexArray::~GlVertexArray()
{
    uint32_t id = m_id;
    Renderer::s_submit([id]() { glDeleteVertexArrays(1, &id); });

}

void GlVertexArray::bind() const
{
    uint32_t id = m_id;
    Renderer::s_submit(
        [id]()
        {
            if (id != s_currBoundId)
            {
                glBindVertexArray(id);

                s_currBoundId = id;
            }
        });
}

void GlVertexArray::unbind() const
{
    Renderer::s_submit([]() { glBindVertexArray(0); });
    s_currBoundId = 0;
}

void GlVertexArray::addVertexBuffer(const ref<VertexBuffer>& p_vertexBuffer)
{
    NM_CORE_ASSERT(p_vertexBuffer->getFormat().getComponents().size(),
                   "VBO format is required to create VBA");

    // ref<GlVertexArray> p_instance       = makeRef<GlVertexArray>(*this);
    GlVertexArray* p_instance = this;


    Renderer::s_submit(
        [p_instance, p_vertexBuffer]()
        {
            glBindVertexArray(p_instance->m_id);
            glBindBuffer(GL_ARRAY_BUFFER, p_vertexBuffer->getId());


            const auto& format = p_vertexBuffer->getFormat();
            for (const auto& component : format)
            {
                uint32_t glType
                    = Shader::s_getShaderType(std::get<0>(component.dataType));

                if (glType == GL_INT || glType == GL_BOOL)
                {
                    glEnableVertexAttribArray(p_instance->m_vertexBufferIndex);

                    uint32_t numOfComponent = std::get<2>(component.dataType);

                    glVertexAttribIPointer(p_instance->m_vertexBufferIndex,
                                           numOfComponent,
                                           glType,
                                           format.getStride(),
                                           (const void*)component.offset);

                    if (component.type == BufferComponent::Type::PER_INSTANCE)
                    {
                        glVertexAttribDivisor(p_instance->m_vertexBufferIndex,
                                              component.perInstance);
                    }
                    p_instance->m_vertexBufferIndex++;
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
                            p_instance->m_vertexBufferIndex);
                        glVertexAttribPointer(
                            p_instance->m_vertexBufferIndex,
                            numOfComponentsPerColumn,
                            glType,
                            component.normalized ? GL_TRUE : GL_FALSE,
                            format.getStride(),
                            (const void*)offset);

                         if (component.type
                            == BufferComponent::Type::PER_INSTANCE)
                        {
                            glVertexAttribDivisor(
                                p_instance->m_vertexBufferIndex,
                                component.perInstance);
                        }
                        p_instance->m_vertexBufferIndex++;
                    }
                }
                else
                {
                    NM_CORE_ASSERT_STATIC(0, "Unknown ShaderDataType!");
                }
            }
        });

    const auto& format = p_vertexBuffer->getFormat();

    uint32_t thisVboVertexCount
        = p_vertexBuffer->getSize() / format.getStride();

    // accumulate the size of the vertex
    m_vertexSize += format.getStride();

    if (m_vertexBuffers.size() == 0)
    {
        // this is our first vertex buffer, so set the expected size in number
        // of vertexes because this "should" to match between all vbos bound to
        // this vba
        m_expectedVboVertexCount = thisVboVertexCount;
    }

    m_vertexBuffers.push_back(p_vertexBuffer);
}

void GlVertexArray::setIndexBuffer(const ref<IndexBuffer>& p_indexBuffer)
{
    // ref<GlVertexArray> p_instance       = makeRef<GlVertexArray>(*this);
    GlVertexArray* p_instance = this;


    Renderer::s_submit(
        [p_instance, p_indexBuffer]()
        {
            glBindVertexArray(p_instance->m_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p_indexBuffer->getId());
        });

    m_indexBuffer = p_indexBuffer;
}

};  // namespace nimbus