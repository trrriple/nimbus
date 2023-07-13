#include "nmpch.hpp"
#include "core.hpp"

#include "resourceManager.hpp"

namespace nimbus
{

ResourceManager& ResourceManager::get()
{
    static ResourceManager instance;
    return instance;
}

// Other member functions and variables...

Texture* ResourceManager::loadTexture(const Texture::Type type,
                                      const std::string&  path,
                                      const bool          flipOnLoad)
{
    NM_PROFILE_DETAIL();
    
    // check to see if it was already loaded
    auto     p_textureEntry = m_loadedTextures.find(path);
    Texture* p_texture      = nullptr;
    if (p_textureEntry != m_loadedTextures.end())
    {
        p_texture = p_textureEntry->second;
    }
    else
    {
        p_texture = new Texture(type, path, flipOnLoad);

        m_loadedTextures[path] = p_texture;

        NM_CORE_INFO("ResourceManager::Texture Name loaded %s\n",
               p_texture->m_path.c_str());
    }

    return p_texture;
}

Shader* ResourceManager::loadShader(const std::string& vertexPath,
                                    const std::string& fragmentPath)
{
    NM_PROFILE_DETAIL();
    
    // check to see if it was already loaded
    std::string path          = vertexPath + fragmentPath;
    auto        p_shaderEntry = m_loadedShaders.find(path);
    Shader*     p_shader      = nullptr;
    if (p_shaderEntry != m_loadedShaders.end())
    {
        p_shader = p_shaderEntry->second;
    }
    else
    {
        p_shader = new Shader(vertexPath, fragmentPath);

        m_loadedShaders[path] = p_shader;

        NM_CORE_INFO(
            "ResourceManager::Shader Compiled from: \n\tVertex:   %s "
            "\n\tFragment: "
            "%s\n",
            p_shader->getVertexPath().c_str(),
            p_shader->getFragmentPath().c_str());
    }

    return p_shader;
}

ResourceManager::ResourceManager()
{
}

// Private destructor
ResourceManager::~ResourceManager()
{
    for (auto& p_texture : m_loadedTextures)
    {
        delete p_texture.second;
    }

    for (auto& p_shader : m_loadedTextures)
    {
        delete p_shader.second;
    }
}

}  // namespace nimbus