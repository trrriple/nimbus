#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/scene/scene.hpp"
#include "nimbus/scene/entity.hpp"

namespace nimbus
{

class SceneSerializer
{
   public:
    SceneSerializer(const ref<Scene>& p_scene);

    void serialize(const std::string& filepath);
    void serializeBin(const std::string& filepath);

    bool deserialize(const std::string& filepath);
    bool deserializeBin(const std::string& filepath);

   private:
    ref<Scene> mp_scene;

    void _deserializeEntity(void* p_entityTbl, Scene* p_scene, const std::string guidStr);

    void _assembleFamilyTree(void* p_entityTbl, const std::string guidStr);

    std::unordered_map<std::string, Entity> m_entityMap;
};

}  // namespace nimbus