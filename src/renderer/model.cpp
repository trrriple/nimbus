#include "renderer/model.hpp"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "core.hpp"
#include "nmpch.hpp"
#include "resourceManager.hpp"

namespace nimbus
{

Model::Model(std::string path, bool flipOnLoad, bool normalize)
    : m_flipOnLoad(flipOnLoad), m_normalize(normalize)
{
    loadModel(path);

    Log::coreInfo("Loaded Textures for model %s", path.c_str());
    for (const auto& pair : m_loadedTextures)
    {
        Log::coreInfo("\t %s", pair.second->getPath().c_str());
    }
}

void Model::draw(ref<Shader>& shader, glm::mat4& model)
{
    for (uint32_t i = 0; i < mp_meshes.size(); i++)
    {
        mp_meshes[i]->setShader(shader);
        mp_meshes[i]->draw(model);
    }
}

void Model::loadModel(std::string path)
{
    Assimp::Importer import;
    const aiScene*   scene
        = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
        || !scene->mRootNode)
    {
        Log::coreError("ASSIMP:: %s", import.GetErrorString());
        return;
    }

    std::filesystem::path filePath(path);
    m_directory = filePath.parent_path().string();

    processNode(static_cast<void*>(scene->mRootNode),
                static_cast<const void*>(scene));
}

void Model::processNode(void* node, const void* scene)
{
    aiNode*        ainode  = static_cast<aiNode*>(node);
    const aiScene* aiScene = static_cast<const struct aiScene*>(scene);

    // process all the node's meshes (if any)
    for (uint32_t i = 0; i < ainode->mNumMeshes; i++)
    {
        aiMesh* mesh = aiScene->mMeshes[ainode->mMeshes[i]];
        mp_meshes.push_back(processMesh(mesh, aiScene));
    }
    // then do the same for each of its children
    for (uint32_t i = 0; i < ainode->mNumChildren; i++)
    {
        processNode(ainode->mChildren[i], scene);
    }
}

scope<Mesh> Model::processMesh(void* mesh, const void* scene)
{
    std::vector<Mesh::Vertex> vertices;
    std::vector<uint32_t>     indices;
    std::vector<ref<Texture>> textures;

    aiMesh*        aimesh  = static_cast<aiMesh*>(mesh);
    const aiScene* aiScene = static_cast<const struct aiScene*>(scene);

    // process vertex positions, normals and texture coordinates
    for (uint32_t i = 0; i < aimesh->mNumVertices; i++)
    {
        Mesh::Vertex vertex;
        // we declare a placeholder vector since assimp uses its own vector
        // class that doesn't directly convert to glm's vec3 class so we
        // transfer the data to this placeholder glm::vec3 first.
        glm::vec3 vector;
        // positions
        vector.x        = aimesh->mVertices[i].x;
        vector.y        = aimesh->mVertices[i].y;
        vector.z        = aimesh->mVertices[i].z;
        vertex.position = vector;
        // normals
        if (aimesh->HasNormals())
        {
            vector.x      = aimesh->mNormals[i].x;
            vector.y      = aimesh->mNormals[i].y;
            vector.z      = aimesh->mNormals[i].z;
            vertex.normal = vector;
        }
        // texture coordinates
        if (aimesh->mTextureCoords[0])  // does the mesh contain texture
                                        // coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates.
            // We thus make the assumption that we won't use models where a
            // vertex can have multiple texture coordinates so we always
            // take the first set (0).
            vec.x            = aimesh->mTextureCoords[0][i].x;
            vec.y            = aimesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
            // tangent
            if (aimesh->HasTangentsAndBitangents())
            {
                vector.x       = aimesh->mTangents[i].x;
                vector.y       = aimesh->mTangents[i].y;
                vector.z       = aimesh->mTangents[i].z;
                vertex.tangent = vector;
                // bitangent
                vector.x         = aimesh->mBitangents[i].x;
                vector.y         = aimesh->mBitangents[i].y;
                vector.z         = aimesh->mBitangents[i].z;
                vertex.bitangent = vector;
            }
        }
        else
        {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // now walk through each of the mesh's faces (a face is a mesh its
    // triangle) and retrieve the corresponding vertex indices.
    for (uint32_t i = 0; i < aimesh->mNumFaces; i++)
    {
        aiFace face = aimesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // process material
    if (aimesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = aiScene->mMaterials[aimesh->mMaterialIndex];

        // 1. diffuse maps
        std::vector<ref<Texture>> diffuseMaps
            = loadMaterialTextures(material, Texture::Type::DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        // 2. specular maps
        std::vector<ref<Texture>> specularMaps
            = loadMaterialTextures(material, Texture::Type::SPECULAR);
        textures.insert(
            textures.end(), specularMaps.begin(), specularMaps.end());

        // 3. Ambient maps
        std::vector<ref<Texture>> ambientMaps
            = loadMaterialTextures(material, Texture::Type::AMBIENT);
        textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());

        // 3. normal maps
        std::vector<ref<Texture>> normalMaps
            = loadMaterialTextures(material, Texture::Type::NORMAL);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        // 4. height maps
        std::vector<ref<Texture>> heightMaps
            = loadMaterialTextures(material, Texture::Type::HEIGHT);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }
    return makeScope<Mesh>(vertices, indices, textures, m_normalize);
}

std::vector<ref<Texture>> Model::loadMaterialTextures(void*         mat,
                                                      Texture::Type texType)

{
    aiMaterial* aimat = static_cast<aiMaterial*>(mat);

    aiTextureType aiType = aiTextureType_NONE;
    if (texType == Texture::Type::NORMAL)
    {
        aiType = aiTextureType_NORMALS;
    }
    else if (texType == Texture::Type::DIFFUSE)
    {
        aiType = aiTextureType_DIFFUSE;
    }
    else if (texType == Texture::Type::SPECULAR)
    {
        aiType = aiTextureType_SPECULAR;
    }
    else if (texType == Texture::Type::AMBIENT)
    {
        aiType = aiTextureType_AMBIENT;
    }
    else if (texType == Texture::Type::HEIGHT)
    {
        aiType = aiTextureType_HEIGHT;
    }
    else
    {
        NM_CORE_ASSERT(0, "Unknown Texture format 0x%x", texType);
    }

    std::vector<ref<Texture>> textures;
    for (unsigned int i = 0; i < aimat->GetTextureCount(aiType); i++)
    {
        aiString filename;
        aimat->GetTexture(aiType, i, &filename);
        std::string path = m_directory + "/" + filename.C_Str();

        bool skip           = false;
        auto p_textureEntry = m_loadedTextures.find(path);

        if (p_textureEntry != m_loadedTextures.end())
        {
            // texture already loaded
            textures.push_back(p_textureEntry->second);
            skip = true;
            break;
        }

        if (!skip)
        {
            ResourceManager& rm = ResourceManager::get();

            ref<Texture>& p_texture
                = rm.loadTexture(texType, path, m_flipOnLoad);

            textures.push_back(p_texture);
            m_loadedTextures.emplace(path, p_texture);
        }
    }
    return textures;
}

}  // namespace nimbus