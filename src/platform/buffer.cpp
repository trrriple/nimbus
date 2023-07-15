#include "nmpch.hpp"
#include "core.hpp"

#include "platform/buffer.hpp"

namespace nimbus
{
////////////////////////////////////////////////////////////////////////////////
// Vertex Buffer
////////////////////////////////////////////////////////////////////////////////
VertexBuffer::VertexBuffer(const void*        vertices,
                           std::uint32_t      size,
                           VertexBuffer::Type type)
    : m_size(size), m_type(type)
{
    glCreateBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);

    switch (m_type)
    {
        case (VertexBuffer::Type::STATIC_DRAW):
        {
            glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
            break;
        }
        case (VertexBuffer::Type::DYNAMIC_DRAW):
        {
            glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
            break;
        }
        case (VertexBuffer::Type::STREAM_DRAW):
        {
            glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STREAM_DRAW);
            break;
        }
    }
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
    : m_count(count)
{
    glCreateBuffers(1, &m_id);

    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER,
                 count * sizeof(std::uint32_t),
                 indices,
                 GL_STATIC_DRAW);

    m_type = GL_UNSIGNED_INT;
}

IndexBuffer::IndexBuffer(std::uint16_t* indices, std::uint16_t count)
    : m_count(count)
{
    glCreateBuffers(1, &m_id);

    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER,
                 count * sizeof(std::uint16_t),
                 indices,
                 GL_STATIC_DRAW);

    m_type = GL_UNSIGNED_SHORT;
}

IndexBuffer::IndexBuffer(std::uint8_t* indices, std::uint8_t count)
    : m_count(count)
{
    glCreateBuffers(1, &m_id);

    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(
        GL_ARRAY_BUFFER, count * sizeof(std::uint8_t), indices, GL_STATIC_DRAW);
    
    m_type = GL_UNSIGNED_BYTE;
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

        if (std::get<0>(component.dataType) == GL_INT
            || std::get<0>(component.dataType) == GL_BOOL)
        {
            glEnableVertexAttribArray(m_vertexBufferIndex);

            std::uint32_t typeOfComponent = std::get<0>(component.dataType);
            std::uint32_t numOfComponent = std::get<2>(component.dataType);
            
            glVertexAttribIPointer(m_vertexBufferIndex,
                                   numOfComponent,
                                   typeOfComponent,
                                   format.getStride(),
                                   (const void*)component.offset);

            if (component.type == BufferComponent::Type::PER_INSTANCE)
            {
                glVertexAttribDivisor(m_vertexBufferIndex,
                                      component.perInstance);
            }
            m_vertexBufferIndex++;
        }
        else if (std::get<0>(component.dataType) == GL_FLOAT)
        {
            std::uint32_t typeOfComponent = std::get<0>(component.dataType);
            std::uint32_t numOfComponent  = std::get<2>(component.dataType);
            std::uint32_t columns         = 1;

            std::uint32_t numOfComponentsPerColumn = numOfComponent;
            if (numOfComponent > 4)
            {
                // This is a matrix we must add a pointer for each
                // column of the matrix
                columns = static_cast<std::uint32_t>(std::sqrt(numOfComponent));
                numOfComponentsPerColumn
                    = columns;  // matrix will always be square
            }

            for (std::uint32_t i = 0; i < columns; i++)
            {
                std::uint64_t offset
                    = component.offset
                      + (sizeof(float) * numOfComponentsPerColumn * i);

                glEnableVertexAttribArray(m_vertexBufferIndex);
                glVertexAttribPointer(m_vertexBufferIndex,
                                      numOfComponentsPerColumn,
                                      typeOfComponent,
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

    std::uint32_t thisVboVertexCount
        = vertexBuffer->getSize() / format.getStride();

    // accumulate the size of the vertex
    m_vertexSize += format.getStride();

    if (m_vertexBuffers.size() == 0)
    {
        // this is our first vertex buffer, so set the expected size in number
        // of vertexes because this "should" to match between all vbos bound to
        // this vba
        m_expectedVboVertexCount = thisVboVertexCount;
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