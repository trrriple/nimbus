#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/scene/scene.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-W#pragma-messages"
#include "yaml-cpp/yaml.h"
#pragma GCC diagnostic pop

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
    
    void _serializeEntity(YAML::Emitter& out, Entity entity);
};

}  // namespace nimbus