#include "nmpch.hpp"
#include "core.hpp"

#include "platform/buffer.hpp"

namespace nimbus
{
////////////////////////////////////////////////////////////////////////////////
// Vertex Buffer
////////////////////////////////////////////////////////////////////////////////
VertexBuffer::VertexBuffer(std::uint32_t size)
{
    glCreateBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

VertexBuffer::VertexBuffer(const void* data, std::uint32_t size)
{
    glCreateBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_id);
}

void VertexBuffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VertexBuffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::setData(const void* data, std::uint32_t size)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}


////////////////////////////////////////////////////////////////////////////////
// Index Buffer
////////////////////////////////////////////////////////////////////////////////
IndexBuffer::IndexBuffer(std::uint32_t* indices, std::uint32_t count)
{
    glCreateBuffers(1, &m_id);

    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER,
                 count * sizeof(std::uint32_t),
                 indices,
                 GL_STATIC_DRAW);
}

IndexBuffer::IndexBuffer(std::uint16_t* indices, std::uint16_t count)
{
    glCreateBuffers(1, &m_id);

    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER,
                 count * sizeof(std::uint16_t),
                 indices,
                 GL_STATIC_DRAW);
}

IndexBuffer::IndexBuffer(std::uint8_t* indices, std::uint8_t count)
{
    glCreateBuffers(1, &m_id);

    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(
        GL_ARRAY_BUFFER, count * sizeof(std::uint8_t), indices, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &m_id);
}

void IndexBuffer::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void IndexBuffer::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


////////////////////////////////////////////////////////////////////////////////
// Vertex Array
////////////////////////////////////////////////////////////////////////////////
VertexArray::VertexArray()
{
    glCreateVertexArrays(1, &m_id);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_id);
}

void VertexArray::bind() const
{
    glBindVertexArray(m_id);
}

void VertexArray::unbind() const
{
    glBindVertexArray(0);
}

void VertexArray::addVertexBuffer(const ref<VertexBuffer>& vertexBuffer)
{
    NM_CORE_ASSERT(vertexBuffer->getFormat().getComponents().size(),
                   "VBO format is required to create VBA\n");

    glBindVertexArray(m_id);
    vertexBuffer->bind();

    const auto& format = vertexBuffer->getFormat();
    for (const auto& component : format)
    {
        glEnableVertexAttribArray(m_vertexBufferIndex);

        if (std::get<0>(component.type) == GL_INT
            || std::get<0>(component.type) == GL_BOOL)
        {
            glVertexAttribIPointer(m_vertexBufferIndex,
                                   std::get<2>(component.type),
                                   std::get<0>(component.type),
                                   format.getStride(),
                                   (const void*)component.offset);
        }
        else if (std::get<0>(component.type) == GL_FLOAT)
        {
            std::uint32_t count   = std::get<2>(component.type);
            std::uint32_t columns = 1;

            if (count > 4)
            {
                // This is a matrix we must add a pointer for each
                // column of the matrix
                columns = static_cast<std::uint32_t>(std::sqrt(count));
                count   = columns;  // matrix will always be square
            }

            for (std::uint32_t i = 0; i < columns; i++)
            {
                std::uint64_t offset
                    = component.offset + (sizeof(float) * count * i);
                glVertexAttribPointer(m_vertexBufferIndex,
                                      count,
                                      std::get<0>(component.type),
                                      component.normalized ? GL_TRUE : GL_FALSE,
                                      format.getStride(),
                                      (const void*)offset);
            }
        }
        else
        {
            NM_CORE_ASSERT(0, "Unknown ShaderDataType!");
        }

        m_vertexBufferIndex++;
    }

    m_vertexBuffers.push_back(vertexBuffer);
}

void VertexArray::setIndexBuffer(const ref<IndexBuffer>& indexBuffer)
{
    glBindVertexArray(m_id);
    indexBuffer->bind();

    m_indexBuffer = indexBuffer;
}


};  // namespace nimbus