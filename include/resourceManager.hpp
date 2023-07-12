#pragma once

#include "core.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace nimbus
{

class ResourceManager
{
   public:
    static ResourceManager& get();

    // Other member functions and variables...

    Texture* loadTexture(const Texture::Type type,
                         const std::string&  path,
                         const bool          flipOnLoad = false);

    Shader* loadShader(const std::string& vertexPath,
                       const std::string& fragmentPath);

   private:
    std::unordered_map<std::string, Texture*> m_loadedTextures;
    std::unordered_map<std::string, Shader*>  m_loadedShaders;

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