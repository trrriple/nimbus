////////////////////////////////////////////////////////////////////////////////
// OpenGL buffer abstrations, highly influenced by the implementation in
// Hazel. Credit where credit is due!
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "core.hpp"
#include "glad.h"

namespace nimbus
{

////////////////////////////////////////////////////////////////////////////////
// Shader Types MetaData
////////////////////////////////////////////////////////////////////////////////
//                  gl Type   sizeof    # of Components
typedef std::tuple<std::uint32_t, std::uint32_t, std::uint32_t> ShaderDataType;

const ShaderDataType k_shaderFloat  = std::make_tuple(GL_FLOAT, 4, 1);
const ShaderDataType k_shaderVec2   = std::make_tuple(GL_FLOAT, 8, 2);
const ShaderDataType k_shaderVec3   = std::make_tuple(GL_FLOAT, 12, 3);
const ShaderDataType k_shaderVec4   = std::make_tuple(GL_FLOAT, 16, 4);
const ShaderDataType k_shaderMat3   = std::make_tuple(GL_FLOAT, 36, 9);
const ShaderDataType k_shaderMat4   = std::make_tuple(GL_FLOAT, 64, 16);
const ShaderDataType k_shaderInt    = std::make_tuple(GL_INT, 4, 1);
const ShaderDataType k_shaderInt2   = std::make_tuple(GL_INT, 8, 2);
const ShaderDataType k_shaderInt3   = std::make_tuple(GL_INT, 12, 3);
const ShaderDataType k_shaderInt4   = std::make_tuple(GL_INT, 16, 4);
const ShaderDataType k_shaderBool   = std::make_tuple(GL_BOOL, 1, 1);

////////////////////////////////////////////////////////////////////////////////
// Description of component in a buffer
////////////////////////////////////////////////////////////////////////////////
struct BufferComponent
{
    ShaderDataType type;
    std::string    name;
    bool           normalized;
    size_t         offset;

    BufferComponent() = default;

    BufferComponent(ShaderDataType     type,
                    const std::string& name,
                    bool               normalized = false)
        : type(type), name(name), normalized(normalized), offset(0)
    {
    }
};

////////////////////////////////////////////////////////////////////////////////
// Describes the format of a buffer using buffer components
////////////////////////////////////////////////////////////////////////////////
class BufferFormat
{
   public:
    BufferFormat()
    {
    }

    BufferFormat(std::initializer_list<BufferComponent> elements)
        : m_elements(elements)
    {
        _genOffsetsAndStride();
    }

    std::uint32_t getStride() const
    {
        return m_stride;
    }
    const std::vector<BufferComponent>& getComponents() const
    {
        return m_elements;
    }

    std::vector<BufferComponent>::iterator begin()
    {
        return m_elements.begin();
    }
    std::vector<BufferComponent>::iterator end()
    {
        return m_elements.end();
    }
    std::vector<BufferComponent>::const_iterator begin() const
    {
        return m_elements.begin();
    }
    std::vector<BufferComponent>::const_iterator end() const
    {
        return m_elements.end();
    }

   private:
    std::vector<BufferComponent> m_elements;
    std::uint32_t                m_stride = 0;

    void _genOffsetsAndStride()
    {
        size_t offset = 0;
        m_stride      = 0;
        for (auto& element : m_elements)
        {
            element.offset = offset;
            offset += std::get<1>(element.type);
            m_stride += std::get<1>(element.type);
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Buffer
////////////////////////////////////////////////////////////////////////////////
class VertexBuffer
{
   public:
    VertexBuffer(std::uint32_t size);
    VertexBuffer(const void* vertices, std::uint32_t size);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;

    void setData(const void* data, std::uint32_t size);

    const BufferFormat& getFormat() const
    {
        return m_format;
    }
    void setFormat(const BufferFormat& format)
    {
        m_format = format;
    }

   private:
    std::uint32_t m_id;
    BufferFormat  m_format;
};

////////////////////////////////////////////////////////////////////////////////
// Index Buffer
////////////////////////////////////////////////////////////////////////////////
class IndexBuffer
{
   public:
    IndexBuffer(std::uint32_t* indices, std::uint32_t count);
    IndexBuffer(std::uint16_t* indices, std::uint16_t count);
    IndexBuffer(std::uint8_t* indices, std::uint8_t count);

    ~IndexBuffer();

    void bind() const;
    void unbind() const;

    std::uint32_t getCount() const
    {
        return m_count;
    }

   private:
    std::uint32_t m_id;
    std::uint32_t m_count;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Array
////////////////////////////////////////////////////////////////////////////////
class VertexArray
{
   public:
    VertexArray();
    ~VertexArray();

    void bind() const;
    void unbind() const;

    void addVertexBuffer(const ref<VertexBuffer>& vertexBuffer);

    void setIndexBuffer(const ref<IndexBuffer>& indexBuffer);

    const std::vector<ref<VertexBuffer>>& getVertexBuffers() const
    {
        return m_vertexBuffers;
    }

    const ref<IndexBuffer>& getIndexBuffer() const
    {
        return m_indexBuffer;
    }

   private:
    uint32_t                       m_id;
    uint32_t                       m_vertexBufferIndex = 0;
    std::vector<ref<VertexBuffer>> m_vertexBuffers;
    ref<IndexBuffer>               m_indexBuffer;
};

};  // namespace nimbus
