////////////////////////////////////////////////////////////////////////////////
// Buffer abstrations, highly influenced by the implementation in
// Hazel. Credit where credit is due!
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/renderer/shader.hpp"

#include <cstdint>
#include <vector>

namespace nimbus
{

////////////////////////////////////////////////////////////////////////////////
// Shader Types MetaData
////////////////////////////////////////////////////////////////////////////////
//                    Type             sizeof      # of Components
typedef std::tuple<Shader::ShaderType, uint32_t, uint32_t> ShaderDataType;

const ShaderDataType k_shaderFloat
    = std::make_tuple(Shader::ShaderType::FLOAT, 4, 1);
const ShaderDataType k_shaderVec2
    = std::make_tuple(Shader::ShaderType::FLOAT, 8, 2);
const ShaderDataType k_shaderVec3
    = std::make_tuple(Shader::ShaderType::FLOAT, 12, 3);
const ShaderDataType k_shaderVec4
    = std::make_tuple(Shader::ShaderType::FLOAT, 16, 4);
const ShaderDataType k_shaderMat3
    = std::make_tuple(Shader::ShaderType::FLOAT, 36, 9);
const ShaderDataType k_shaderMat4
    = std::make_tuple(Shader::ShaderType::FLOAT, 64, 16);
const ShaderDataType k_shaderInt
    = std::make_tuple(Shader::ShaderType::INT, 4, 1);
const ShaderDataType k_shaderInt2
    = std::make_tuple(Shader::ShaderType::INT, 8, 2);
const ShaderDataType k_shaderInt3
    = std::make_tuple(Shader::ShaderType::INT, 12, 3);
const ShaderDataType k_shaderInt4
    = std::make_tuple(Shader::ShaderType::INT, 16, 4);
const ShaderDataType k_shaderBool
    = std::make_tuple(Shader::ShaderType::BOOL, 1, 1);

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
    uint32_t              perInstance;
    bool                  normalized;
    size_t                offset;  // calculated

    BufferComponent() = default;

    BufferComponent(ShaderDataType     dataType,
                    const std::string& name,
                    bool               normalized = false)
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
                    uint32_t              perInstance,
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

    uint32_t getStride() const
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
    uint32_t                     m_stride = 0;

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
class VertexBuffer : public refCounted
{
   public:
    enum class Type
    {
        STATIC_DRAW,
        DYNAMIC_DRAW,
        STREAM_DRAW,
    };

    static ref<VertexBuffer> s_create(const void*        vertices,
                                      uint32_t           size,
                                      VertexBuffer::Type type
                                      = VertexBuffer::Type::STATIC_DRAW);

    virtual ~VertexBuffer() = default;

    virtual void bind() const = 0;

    virtual void unbind() const = 0;

    virtual void setData(const void* data, uint32_t size) = 0;

    virtual const BufferFormat& getFormat() const = 0;

    virtual void setFormat(const BufferFormat& format) = 0;

    virtual uint32_t getSize() const
    {
        return m_size;
    }

    virtual uint32_t getId() const
    {
        return m_id;
    }

   protected:
    uint32_t           m_id;
    void*              mp_memory;
    uint32_t           m_size;  // in bytes
    VertexBuffer::Type m_type;
    BufferFormat       m_format;
};

////////////////////////////////////////////////////////////////////////////////
// Index Buffer
////////////////////////////////////////////////////////////////////////////////
class IndexBuffer : public refCounted
{
   public:
    static ref<IndexBuffer> s_create(uint32_t* indices, uint32_t count);
    static ref<IndexBuffer> s_create(uint16_t* indices, uint32_t count);
    static ref<IndexBuffer> s_create(uint8_t* indices, uint32_t count);

    virtual ~IndexBuffer() = default;

    virtual void bind() const = 0;

    virtual void unbind() const = 0;

    virtual uint32_t getCount() const = 0;

    virtual uint32_t getType() const = 0;

    virtual uint32_t getId() const
    {
        return m_id;
    }

   protected:
    uint32_t m_id;
    uint32_t m_count;
    uint32_t m_type;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Array
////////////////////////////////////////////////////////////////////////////////
class VertexArray : public refCounted
{
   public:
    static ref<VertexArray> s_create();

    virtual ~VertexArray() = default;

    virtual void bind() const = 0;

    virtual void unbind() const = 0;

    virtual void addVertexBuffer(ref<VertexBuffer> p_vertexBuffer) = 0;

    virtual void setIndexBuffer(ref<IndexBuffer> p_indexBuffer) = 0;

    virtual const std::vector<ref<VertexBuffer>>& getVertexBuffers() const = 0;

    virtual const ref<IndexBuffer>& getIndexBuffer() const = 0;

    virtual uint32_t getExpectedVertexCount() = 0;

   protected:
    uint32_t                       m_id;
    uint32_t                       m_vertexBufferIndex = 0;
    std::vector<ref<VertexBuffer>> m_vertexBuffers;
    ref<IndexBuffer>               m_indexBuffer            = nullptr;
    uint32_t                       m_expectedVboVertexCount = 0;
    uint32_t                       m_vertexSize             = 0;
};

}  // namespace nimbus
