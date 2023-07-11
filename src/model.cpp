#include "model.hpp"

namespace nimbus
{

Model::Model(std::string path, bool flipOnLoad, bool normalize)
    : m_flipOnLoad(flipOnLoad), m_normalize(normalize)
{
    loadModel(path);

    NM_LOG("Loaded Textures for model %s\n", path.c_str());
    for (const auto& pair : m_loadedTextures)
    {
        NM_LOG("\t %s\n", pair.second->m_path.c_str());
    }
}

void Model::draw(Shader& shader)
{
    for (uint32_t i = 0; i < m_meshes.size(); i++)
    {
        m_meshes[i].setShader(&shader);
        m_meshes[i].draw();
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
        NM_ELOG(0, "ASSIMP:: %s\n", import.GetErrorString());
        return;
    }

    std::filesystem::path filePath(path);
    m_directory = filePath.parent_path().string();

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture*> textures;

    // process vertex positions, normals and texture coordinates
    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // we declare a placeholder vector since assimp uses its own vector
        // class that doesn't directly convert to glm's vec3 class so we
        // transfer the data to this placeholder glm::vec3 first.
        glm::vec3 vector;
        // positions
        vector.x        = mesh->mVertices[i].x;
        vector.y        = mesh->mVertices[i].y;
        vector.z        = mesh->mVertices[i].z;
        vertex.position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x      = mesh->mNormals[i].x;
            vector.y      = mesh->mNormals[i].y;
            vector.z      = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0])  // does the mesh contain texture
                                      // coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates.
            // We thus make the assumption that we won't use models where a
            // vertex can have multiple texture coordinates so we always
            // take the first set (0).
            vec.x            = mesh->mTextureCoords[0][i].x;
            vec.y            = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
            // tangent
            if (mesh->HasTangentsAndBitangents())
            {
                vector.x       = mesh->mTangents[i].x;
                vector.y       = mesh->mTangents[i].y;
                vector.z       = mesh->mTangents[i].z;
                vertex.tangent = vector;
                // bitangent
                vector.x         = mesh->mBitangents[i].x;
                vector.y         = mesh->mBitangents[i].y;
                vector.z         = mesh->mBitangents[i].z;
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
    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // process material
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // 1. diffuse maps
        std::vector<Texture*> diffuseMaps
            = loadMaterialTextures(material, Texture::Type::DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        // 2. specular maps
        std::vector<Texture*> specularMaps
            = loadMaterialTextures(material, Texture::Type::SPECULAR);
        textures.insert(
            textures.end(), specularMaps.begin(), specularMaps.end());

        // 3. Ambient maps
        std::vector<Texture*> ambientMaps
            = loadMaterialTextures(material, Texture::Type::AMBIENT);
        textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());

        // 3. normal maps
        std::vector<Texture*> normalMaps
            = loadMaterialTextures(material, Texture::Type::NORMAL);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        // 4. height maps
        std::vector<Texture*> heightMaps
            = loadMaterialTextures(material, Texture::Type::HEIGHT);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }
    return Mesh(vertices, indices, textures, m_normalize);
}

std::vector<Texture*> Model::loadMaterialTextures(aiMaterial*   mat,
                                                  Texture::Type texType)

{
    aiTextureType aiType;
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
        throw std::invalid_argument("Unknown Texture Type");
    }

    std::vector<Texture*> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(aiType); i++)
    {
        aiString filename;
        mat->GetTexture(aiType, i, &filename);
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
            ResourceManager& rm = ResourceManager::getInstance();

            Texture* p_texture = rm.loadTexture(texType, path, m_flipOnLoad);

            textures.push_back(p_texture);
            m_loadedTextures.emplace(path, p_texture);
        }
    }
    return textures;
}

}  // namespace nimbus