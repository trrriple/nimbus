#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/core/resourceManager.hpp"

#include "nimbus/renderer/texture.hpp"
#include "nimbus/renderer/shader.hpp"
#include "nimbus/renderer/font.hpp"

namespace nimbus
{

ResourceManager& ResourceManager::s_get()
{
    static ResourceManager instance;
    return instance;
}

ref<Texture> ResourceManager::loadTexture(const Texture::Type type,
                                          const std::string&  path,
                                          const bool          flipOnLoad)
{
    NM_PROFILE_DETAIL();

    std::filesystem::path filePath(path);

    std::string relativePath;

    if (filePath.root_name() != std::filesystem::current_path().root_name())
    {
        // Paths are on different drives
        relativePath = filePath.generic_string();
    }
    else
    {
        relativePath = std::filesystem::relative(filePath).generic_string();
    }

    // check to see if it was already loaded
    auto p_textureEntry = m_loadedTextures.find(relativePath);
    if (p_textureEntry != m_loadedTextures.end())
    {
        return p_textureEntry->second;
    }
    else
    {
        ref<Texture> texture
            = Texture::s_create(type, relativePath, flipOnLoad);

        if (texture != nullptr)
        {
            auto texturePair = m_loadedTextures.emplace(relativePath, texture);

            Log::coreInfo("ResourceManager::Texture loaded %s, format %i",
                          texturePair.first->second->getPath().c_str(),
                          texturePair.first->second->getSpec().format);

            return texturePair.first->second;
        }
        else
        {
            return nullptr;
        }
    }
}

ref<Shader> ResourceManager::loadShader(const std::string& name,
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
            = Shader::s_create(name, vertexSource, fragmentSource);

        auto shaderPair
            = m_loadedShaders.emplace(p_shader->getName(), p_shader);

        Log::coreInfo("ResourceManager::Shader %s Compiled",
                      shaderPair.first->second->getName().c_str());

        return shaderPair.first->second;
    }
}

ref<Shader> ResourceManager::loadShader(const std::string& vertexPath,
                                        const std::string& fragmentPath)
{
    NM_PROFILE_DETAIL();

    // check to see if it was already loaded
    std::filesystem::path vFilePath(vertexPath);
    std::filesystem::path fFilePath(fragmentPath);

    std::string vRelativePath;

    std::string fRelativePath;

    if (vFilePath.root_name() != std::filesystem::current_path().root_name())
    {
        // Paths are on different drives
        vRelativePath = vFilePath.generic_string();
    }
    else
    {
        vRelativePath = std::filesystem::relative(vFilePath).generic_string();
    }

    if (fFilePath.root_name() != std::filesystem::current_path().root_name())
    {
        // Paths are on different drives
        fRelativePath = fFilePath.generic_string();
    }
    else
    {
        fRelativePath = std::filesystem::relative(fFilePath).generic_string();
    }

    std::string name          = vRelativePath + fRelativePath;
    auto        p_shaderEntry = m_loadedShaders.find(name);
    if (p_shaderEntry != m_loadedShaders.end())
    {
        return p_shaderEntry->second;
    }
    else
    {
        auto shaderPair = m_loadedShaders.emplace(
            name, Shader::s_create(vRelativePath, fRelativePath));

        Log::coreInfo(
            "ResourceManager::Shader Compiled from: \n\tVertex:   %s "
            "\n\tFragment: "
            "%s",
            shaderPair.first->second->getVertexPath().c_str(),
            shaderPair.first->second->getFragmentPath().c_str());

        return shaderPair.first->second;
    }
}

ref<Font> ResourceManager::loadFont(const std::string& path)
{
    NM_PROFILE_DETAIL();

    // check to see if it was already loaded
    auto p_fontEntry = m_loadedFonts.find(path);
    if (p_fontEntry != m_loadedFonts.end())
    {
        return p_fontEntry->second;
    }
    else
    {
        std::filesystem::path filePath(path);

        ref<Font> font = Font::s_create(filePath.generic_string());

        auto fontPair = m_loadedFonts.emplace(path, font);

        Log::coreInfo("ResourceManager::Font loaded %s",
                      fontPair.first->second->getPath().c_str());

        return fontPair.first->second;
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