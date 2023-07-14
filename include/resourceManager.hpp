#pragma once

#include "renderer/shader.hpp"
#include "renderer/texture.hpp"
#include "common.hpp"

#include <unordered_map>
#include <string>

namespace nimbus
{

class ResourceManager
{
   public:
    static ResourceManager& get();

    // Other member functions and variables...

    ref<Texture>& loadTexture(const Texture::Type type,
                               const std::string&  path,
                               const bool          flipOnLoad = false);

    ref<Shader>& loadShader(const std::string& vertexPath,
                            const std::string& fragmentPath);

   private:
    std::unordered_map<std::string, ref<Texture>> m_loadedTextures;
    std::unordered_map<std::string, ref<Shader>>  m_loadedShaders;

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