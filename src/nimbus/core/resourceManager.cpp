#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/core/resourceManager.hpp"

#include "nimbus/renderer/texture.hpp"
#include "nimbus/renderer/shader.hpp"


namespace nimbus
{

ResourceManager& ResourceManager::s_get()
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
            path, Texture::s_create(type, path, flipOnLoad));

        Log::coreInfo("ResourceManager::Texture loaded %s, format %i",
                     texturePair.first->second->getPath().c_str(),
                     texturePair.first->second->getSpec().format);

        return texturePair.first->second;
    }
}

ref<Shader>& ResourceManager::loadShader(const std::string& name,
                                         const std::string& vertexSource,
                                         const std::string& fragmentSource)
{
    NM_PROFILE_DETAIL();

    // check to see if it was already loaded
    auto p_shaderEntry = m_loadedShaders.find(name);
    if (p_shaderEntry != m_loadedShaders.end())
    {
        return p_shaderEntry->second;
    }
    else
    {
        ref<Shader> p_shader
            = Shader::create(name, vertexSource, fragmentSource);

        auto shaderPair
            = m_loadedShaders.emplace(p_shader->getName(), p_shader);

        Log::coreInfo("ResourceManager::Shader %s Compiled",
                     shaderPair.first->second->getName().c_str());

        return shaderPair.first->second;
    }
}

ref<Shader>& ResourceManager::loadShader(const std::string& vertexPath,
                                         const std::string& fragmentPath)
{
    NM_PROFILE_DETAIL();

    // check to see if it was already loaded
    std::string name          = vertexPath + fragmentPath;
    auto        p_shaderEntry = m_loadedShaders.find(name);
    if (p_shaderEntry != m_loadedShaders.end())
    {
        return p_shaderEntry->second;
    }
    else
    {
        auto shaderPair = m_loadedShaders.emplace(
            name, Shader::create(vertexPath, fragmentPath));

        Log::coreInfo(
            "ResourceManager::Shader Compiled from: \n\tVertex:   %s "
            "\n\tFragment: "
            "%s",
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