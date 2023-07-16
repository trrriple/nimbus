////////////////////////////////////////////////////////////////////////////////
// OpenGL buffer abstrations, highly influenced by the implementation in
// Hazel. Credit where credit is due!
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <vector>

#include "common.hpp"
#include "glad.h"

namespace nimbus
{

////////////////////////////////////////////////////////////////////////////////
// Shader Types MetaData
////////////////////////////////////////////////////////////////////////////////
//                     gl Type       sizeof      # of Components
typedef std::tuple<std::uint32_t, std::uint32_t, std::uint32_t> ShaderDataType;

const ShaderDataType k_shaderFloat = std::make_tuple(GL_FLOAT, 4, 1);
const ShaderDataType k_shaderVec2  = std::make_tuple(GL_FLOAT, 8, 2);
const ShaderDataType k_shaderVec3  = std::make_tuple(GL_FLOAT, 12, 3);
const ShaderDataType k_shaderVec4  = std::make_tuple(GL_FLOAT, 16, 4);
const ShaderDataType k_shaderMat3  = std::make_tuple(GL_FLOAT, 36, 9);
const ShaderDataType k_shaderMat4  = std::make_tuple(GL_FLOAT, 64, 16);
const ShaderDataType k_shaderInt   = std::make_tuple(GL_INT, 4, 1);
const ShaderDataType k_shaderInt2  = std::make_tuple(GL_INT, 8, 2);
const ShaderDataType k_shaderInt3  = std::make_tuple(GL_INT, 12, 3);
const ShaderDataType k_shaderInt4  = std::make_tuple(GL_INT, 16, 4);
const ShaderDataType k_shaderBool  = std::make_tuple(GL_BOOL, 1, 1);

////////////////////////////////////////////////////////////////////////////////
// Description of component in a buffer
////////////////////////////////////////////////////////////////////////////////
struct BufferComponent
{
    enum class Type
    {
        PER_VERTEX,
        PER_INSTANCE
    };

    ShaderDataType        dataType;
    std::string           name;
    BufferComponent::Type type;
    std::uint32_t         perInstance;
    bool                  normalized;
    size_t                offset; // calculated

    BufferComponent() = default;

    BufferComponent(ShaderDataType        dataType,
                    const std::string&    name,
                    bool                  normalized = false)
        : dataType(dataType),
          name(name),
          type(BufferComponent::Type::PER_VERTEX),
          perInstance(0),
          normalized(normalized),
          offset(0)
    {
    }

    BufferComponent(ShaderDataType        dataType,
                    const std::string&    name,
                    BufferComponent::Type type,
                    std::uint32_t         perInstance,
                    bool                  normalized = false)
        : dataType(dataType),
          name(name),
          type(type),
          perInstance(perInstance),
          normalized(normalized),
          offset(0)
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

    BufferFormat(std::initializer_list<BufferComponent> components)
        : m_components(components)
    {
        _genOffsetsAndStride();
    }

    std::uint32_t getStride() const
    {
        return m_stride;
    }
    const std::vector<BufferComponent>& getComponents() const
    {
        return m_components;
    }

    std::vector<BufferComponent>::iterator begin()
    {
        return m_components.begin();
    }
    std::vector<BufferComponent>::iterator end()
    {
        return m_components.end();
    }
    std::vector<BufferComponent>::const_iterator begin() const
    {
        return m_components.begin();
    }
    std::vector<BufferComponent>::const_iterator end() const
    {
        return m_components.end();
    }

   private:
    std::vector<BufferComponent> m_components;
    std::uint32_t                m_stride = 0;

    void _genOffsetsAndStride()
    {
        size_t offset = 0;
        m_stride      = 0;
        for (auto& component : m_components)
        {
            component.offset = offset;
            offset += std::get<1>(component.dataType);
            m_stride += std::get<1>(component.dataType);
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Buffer
////////////////////////////////////////////////////////////////////////////////
class VertexBuffer
{
   public:
    enum class Type
    {
        STATIC_DRAW,
        DYNAMIC_DRAW,
        STREAM_DRAW,
    };

    VertexBuffer(const void*        vertices,
                 std::uint32_t      size,
                 VertexBuffer::Type type = VertexBuffer::Type::STATIC_DRAW);

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

    std::uint32_t getSize()
    {
        return m_size;
    }

    std::uint32_t getId()
    {
        return m_id;
    }

   private:
    std::uint32_t      m_id;
    std::uint32_t      m_size;  // in bytes
    VertexBuffer::Type m_type;
    BufferFormat       m_format;

    inline static std::uint32_t m_currBoundId = 0;

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

    std::uint32_t getType() const
    {
        return m_type;
    }

   private:
    std::uint32_t m_id;
    std::uint32_t m_count;
    std::uint32_t m_type;
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

    std::uint32_t getExpectedVertexCount()
    {
        return m_expectedVboVertexCount;
    }

   private:
    std::uint32_t                  m_id;
    std::uint32_t                  m_vertexBufferIndex = 0;
    std::vector<ref<VertexBuffer>> m_vertexBuffers;
    ref<IndexBuffer>               m_indexBuffer            = nullptr;
    std::uint32_t                  m_expectedVboVertexCount = 0;
    std::uint32_t                  m_vertexSize             = 0;

};

}  // namespace nimbus
