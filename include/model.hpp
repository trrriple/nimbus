#pragma once

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "core.hpp"
#include "mesh.hpp"
#include "resourceManager.hpp"

namespace nimbus
{

class Model
{
   public:
    bool m_flipOnLoad;
    bool m_normalize;

    Model(std::string path, bool flipOnLoad = false, bool normalize = false);

    void draw(Shader& shader);

   private:
    std::vector<Mesh> m_meshes;
    std::string       m_directory;

    // This allows us to keep track of textures that are already loaded
    // in this model so we can reuse them instead of reloading them
    std::unordered_map<std::string, Texture*> m_loadedTextures;

    void loadModel(std::string path);

    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture*> loadMaterialTextures(aiMaterial*   mat,
                                               Texture::Type texType);
};
}  // namespace nimbus