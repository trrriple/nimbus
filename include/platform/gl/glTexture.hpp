#pragma once
#include "nimbus/renderer/texture.hpp"
#include "nimbus/core/common.hpp"

#include <cstdint>
#include <string>

namespace nimbus
{

class GlTexture : public Texture
{
   public:
    GlTexture(const Type type, Spec& spec, bool submitForMe = true) noexcept;

    GlTexture(const Type         type,
              const std::string& path,
              const bool         flipOnLoad = false) noexcept;

    ~GlTexture() noexcept;

    virtual void bind(const uint32_t glTextureUnit) const noexcept override;

    virtual void unbind() const noexcept override;

    virtual void setData(void* data, uint32_t size) noexcept override;

    virtual uint32_t getId() const noexcept override
    {
        return m_id;
    }

    virtual uint32_t getWidth() const noexcept override
    {
        return m_spec.width;
    }

    virtual uint32_t getHeight() const noexcept override
    {
        return m_spec.height;
    }

    virtual Type getType() const noexcept override
    {
        return m_type;
    }

    virtual const std::string& getPath() const noexcept override
    {
        return m_path;
    }

    virtual const Spec& getSpec() const noexcept override
    {
        return m_spec;
    }

    virtual bool operator==(const Texture& other) const noexcept override
    {
        return (m_id == other.getId());
    }

    static uint32_t s_format(Format format) noexcept;

    static uint32_t s_formatInternal(FormatInternal format) noexcept;

    static uint32_t s_dataType(DataType dataType) noexcept;

    static uint32_t s_filterType(FilterType filterType) noexcept;

    static uint32_t s_wrapType(WrapType wrapType) noexcept;

   private:
    void _storage() noexcept;

    static void _s_gen(uint32_t& id, bool multisample = false) noexcept;
};

}  // namespace nimbus