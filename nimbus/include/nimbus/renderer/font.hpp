#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/renderer/texture.hpp"

#include <thread>
#include <atomic>

namespace nimbus
{
struct FontData;

class Font : public refCounted
{
   public:
    struct Format
    {
        ref<Font> p_font  = nullptr;
        glm::vec4 fgColor = glm::vec4(1.0f);
        glm::vec4 bgColor = glm::vec4(0.0f);
        f32_t     kerning = 0.0f;
        f32_t     leading = 0.0f;
    };

    ~Font();

    inline const std::string& getPath() const
    {
        return m_path;
    }

    inline ref<Texture> getAtlasTex() const
    {
        return m_atlasTex;
    }

    inline const FontData* getFontData() const
    {
        return m_data;
    }

    inline bool isLoaded() const
    {
        // short circuit atomic check if we know it's been
        // loaded already
        if (m_loaded)
        {
            return true;
        }
        else if (m_isDone.load())
        {
            const_cast<Font*>(this)->_initializeTexture();
            return true;
        }
        else
        {
            return false;
        }
    }

   private:
    std::string  m_path;
    FontData*    m_data;
    ref<Texture> m_atlasTex = nullptr;

    // Atomic variable to indicate if processing is done
    std::atomic_bool m_isDone = false;
    std::thread      m_workerThread;

    mutable bool m_loaded = false;

    // only resouce manager can generate the fonts
    Font(const std::string& fontPath);

    static ref<Font> s_create(const std::string& fontPath);

    void _loadFont();
    void _initializeTexture();

    friend class ResourceManager;
};

}  // namespace nimbus