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
    GlTexture(const Type type, Spec& spec, bool submitForMe = true);

    GlTexture(const Type type, const std::string& path, const bool flipOnLoad = false);

    ~GlTexture();

    virtual bool bind(const u32_t glTextureUnit) const override;

    virtual void unbind() const override;

    virtual void setData(void* data, u32_t size) override;

    virtual u32_t getId() const override
    {
        return m_id;
    }

    virtual u32_t getWidth() const override
    {
        return m_spec.width;
    }

    virtual u32_t getHeight() const override
    {
        return m_spec.height;
    }

    virtual Type getType() const override
    {
        return m_type;
    }

    virtual const std::string& getPath() const override
    {
        return m_path;
    }

    virtual const Spec& getSpec() const override
    {
        return m_spec;
    }

    virtual bool operator==(const Texture& other) const override
    {
        return (m_id == other.getId());
    }

    static u32_t s_format(Format format);

    static u32_t s_formatInternal(FormatInternal format);

    static u32_t s_dataType(DataType dataType);

    static u32_t s_filterType(FilterType filterType);

    static u32_t s_wrapType(WrapType wrapType);

   private:
    void _storage();

    static void _s_gen(u32_t& id, bool multisample = false);
};

}  // namespace nimbus