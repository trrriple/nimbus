#include "nimbus/nmpch.hpp"
#include "nimbus/core.hpp"

#include "platform/gl/glBuffer.hpp"

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

    glCreateBuffers(1, &m_id);

    switch (m_type)
    {
        case (VertexBuffer::Type::STATIC_DRAW):
        {
            glNamedBufferData(m_id, size, vertices, GL_STATIC_DRAW);
            break;
        }
        case (VertexBuffer::Type::DYNAMIC_DRAW):
        {
            glNamedBufferData(m_id, size, vertices, GL_DYNAMIC_DRAW);
            break;
        }
        case (VertexBuffer::Type::STREAM_DRAW):
        {
            glNamedBufferData(m_id, size, vertices, GL_STREAM_DRAW);
            break;
        }
    }
}

GlVertexBuffer::~GlVertexBuffer()
{
    unbind();
    glDeleteBuffers(1, &m_id);
}

void GlVertexBuffer::bind() const
{
    if (m_id != m_currBoundId)
    {
        // only bind if this is a different buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
        m_currBoundId = m_id;
    }
}

void GlVertexBuffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_currBoundId = 0;
}

void GlVertexBuffer::setData(const void* data, uint32_t size)
{
    glNamedBufferSubData(m_id, 0, size, data);
}

////////////////////////////////////////////////////////////////////////////////
// Index Buffer
////////////////////////////////////////////////////////////////////////////////
GlIndexBuffer::GlIndexBuffer(uint32_t* indices, uint32_t count)
{
    m_count = count;

    glCreateBuffers(1, &m_id);

    glNamedBufferData(m_id, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);

    m_type = GL_UNSIGNED_INT;
}

GlIndexBuffer::GlIndexBuffer(uint16_t* indices, uint32_t count)
{
    m_count = count;

    glCreateBuffers(1, &m_id);

    glNamedBufferData(m_id, count * sizeof(uint16_t), indices, GL_STATIC_DRAW);

    m_type = GL_UNSIGNED_SHORT;
}

GlIndexBuffer::GlIndexBuffer(uint8_t* indices, uint32_t count)
{
    m_count = count;

    glCreateBuffers(1, &m_id);

    glNamedBufferData(m_id, count * sizeof(uint8_t), indices, GL_STATIC_DRAW);

    m_type = GL_UNSIGNED_BYTE;
}

GlIndexBuffer::~GlIndexBuffer()
{
    unbind();
    glDeleteBuffers(1, &m_id);
}

void GlIndexBuffer::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void GlIndexBuffer::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

////////////////////////////////////////////////////////////////////////////////
// Vertex Array
////////////////////////////////////////////////////////////////////////////////
GlVertexArray::GlVertexArray()
{
    glCreateVertexArrays(1, &m_id);
}

GlVertexArray::~GlVertexArray()
{
    unbind();
    glDeleteVertexArrays(1, &m_id);
}

void GlVertexArray::bind() const
{
    if (m_id != s_currBoundId)
    {
        // only bind if this is a different array
        glBindVertexArray(m_id);
        s_currBoundId = m_id;
    }
}

void GlVertexArray::unbind() const
{
    glBindVertexArray(0);
    s_currBoundId = 0;
}

void GlVertexArray::addVertexBuffer(const ref<VertexBuffer>& p_vertexBuffer)
{
    NM_CORE_ASSERT(p_vertexBuffer->getFormat().getComponents().size(),
                   "VBO format is required to create VBA");

    bind();
    p_vertexBuffer->bind();

    const auto& format = p_vertexBuffer->getFormat();
    for (const auto& component : format)
    {
        uint32_t glType
            = Shader::s_getShaderType(std::get<0>(component.dataType));

        if (glType == GL_INT || glType == GL_BOOL)
        {
            glEnableVertexAttribArray(m_vertexBufferIndex);

            uint32_t numOfComponent = std::get<2>(component.dataType);

            glVertexAttribIPointer(m_vertexBufferIndex,
                                   numOfComponent,
                                   glType,
                                   format.getStride(),
                                   (const void*)component.offset);

            if (component.type == BufferComponent::Type::PER_INSTANCE)
            {
                glVertexAttribDivisor(m_vertexBufferIndex,
                                      component.perInstance);
            }
            m_vertexBufferIndex++;
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
                columns = static_cast<uint32_t>(std::sqrt(numOfComponent));
                numOfComponentsPerColumn
                    = columns;  // matrix will always be square
            }

            for (uint32_t i = 0; i < columns; i++)
            {
                uint64_t offset
                    = component.offset
                      + (sizeof(float) * numOfComponentsPerColumn * i);

                glEnableVertexAttribArray(m_vertexBufferIndex);
                glVertexAttribPointer(m_vertexBufferIndex,
                                      numOfComponentsPerColumn,
                                      glType,
                                      component.normalized ? GL_TRUE : GL_FALSE,
                                      format.getStride(),
                                      (const void*)offset);

                if (component.type == BufferComponent::Type::PER_INSTANCE)
                {
                    glVertexAttribDivisor(m_vertexBufferIndex,
                                          component.perInstance);
                }
                m_vertexBufferIndex++;
            }
        }
        else
        {
            NM_CORE_ASSERT(0, "Unknown ShaderDataType!");
        }
    }

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
    bind();
    p_indexBuffer->bind();

    m_indexBuffer = p_indexBuffer;
}

};  // namespace nimbus