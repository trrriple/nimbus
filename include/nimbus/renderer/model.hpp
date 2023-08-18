#pragma once

#include <string>

#include "common.hpp"
#include "mesh.hpp"
#include "texture.hpp"

namespace nimbus
{

class Model : public refCounted
{
   public:
    bool m_flipOnLoad;
    bool m_normalize;

    Model(std::string path, bool flipOnLoad = false, bool normalize = false);

    void draw(ref<Shader>& shader, const glm::mat4& model);

   private:
    std::vector<scope<Mesh>> mp_meshes;
    std::string              m_directory;

    // This allows us to keep track of textures that are already loaded
    // in this model so we can reuse them instead of reloading them
    std::unordered_map<std::string, ref<Texture>> m_loadedTextures;

    void loadModel(std::string path);

    void processNode(void* node, const void* scene);

    scope<Mesh> processMesh(void* mesh, const void* scene);

    std::vector<ref<Texture>> loadMaterialTextures(void* mat, Texture::Type texType);
};
}  // namespace nimbus