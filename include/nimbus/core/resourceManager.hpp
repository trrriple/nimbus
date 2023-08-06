#pragma once

#include <string>
#include <unordered_map>

#include "nimbus/core/common.hpp"
#include "nimbus/renderer/shader.hpp"
#include "nimbus/renderer/texture.hpp"
#include "nimbus/renderer/font.hpp"

namespace nimbus
{

class ResourceManager
{
   public:
    static ResourceManager& s_get();

    // Other member functions and variables...

    ref<Texture> loadTexture(const Texture::Type type,
                             const std::string&  path,
                             const bool          flipOnLoad = false);

    ref<Shader> loadShader(const std::string& name,
                           const std::string& vertexSource,
                           const std::string& fragmentSource);

    ref<Shader> loadShader(const std::string& vertexPath,
                           const std::string& fragmentPath);

    ref<Font> loadFont(const std::string& path);
   
   private:
    std::unordered_map<std::string, ref<Texture>> m_loadedTextures;
    std::unordered_map<std::string, ref<Shader>>  m_loadedShaders;
    std::unordered_map<std::string, ref<Font>>    m_loadedFonts;

    // Private constructor to prevent direct instantiation
    ResourceManager();

    // Private destructor
    ~ResourceManager();

    // Disable copy constructor
    ResourceManager(const ResourceManager&) = delete;

    // Disable assignment operator
    ResourceManager& operator=(const ResourceManager&) = delete;
};
}  // namespace nimbus