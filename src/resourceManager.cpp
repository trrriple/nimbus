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

ref<Texture>& ResourceManager::loadTexture(const Texture::Type type,
                                           const std::string&  path,
                                           const bool          flipOnLoad)
{
    NM_PROFILE_DETAIL();

    // check to see if it was already loaded
    auto p_textureEntry = m_loadedTextures.find(path);
    if (p_textureEntry != m_loadedTextures.end())
    {
        return p_textureEntry->second;
    }
    else
    {
        auto texturePair = m_loadedTextures.emplace(
            path, makeRef<Texture>(type, path, flipOnLoad));

        NM_CORE_INFO("ResourceManager::Texture loaded %s\n",
                     texturePair.first->second->m_path.c_str());

        return texturePair.first->second;
    }
}

ref<Shader>& ResourceManager::loadShader(const std::string& vertexPath,
                                         const std::string& fragmentPath)
{
    NM_PROFILE_DETAIL();

    // check to see if it was already loaded
    std::string  path          = vertexPath + fragmentPath;
    auto         p_shaderEntry = m_loadedShaders.find(path);
    if (p_shaderEntry != m_loadedShaders.end())
    {
        return p_shaderEntry->second;
    }
    else
    {
        auto shaderPair = m_loadedShaders.emplace(
            path, makeRef<Shader>(vertexPath, fragmentPath));

        NM_CORE_INFO(
            "ResourceManager::Shader Compiled from: \n\tVertex:   %s "
            "\n\tFragment: "
            "%s\n",
            shaderPair.first->second->getVertexPath().c_str(),
            shaderPair.first->second->getFragmentPath().c_str());

        return shaderPair.first->second;
    }
}

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
    for (auto& p_texture : m_loadedTextures)
    {
        p_texture.second.reset();
    }

    for (auto& p_shader : m_loadedTextures)
    {
        p_shader.second.reset();
    }
}

}  // namespace nimbus