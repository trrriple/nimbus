#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/scene/sceneSerializer.hpp"
#include "nimbus/scene/scene.hpp"
#include "nimbus/scene/entity.hpp"
#include "nimbus/scene/component.hpp"

#include "yaml-cpp/yaml.h"

#include <fstream>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////
// Helper macros for yaml-cpp stream syntax being painfully verbose
////////////////////////////////////////////////////////////////////////////////
#define KEY(x)          << YAML::Key << x
#define VAL(x)          << YAML::Value << x
#define KEYVAL(x, y)    KEY(x) VAL(y)
#define MAP_START       << YAML::BeginMap
#define MAP_END         << YAML::EndMap
#define SEQ_START       << YAML::BeginSeq
#define SEQ_END         << YAML::EndSeq

namespace nimbus
{


// glm type overloads
YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
{
    out << YAML::Flow;
    out SEQ_START << v.x << v.y SEQ_END;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
{
    out << YAML::Flow;
    out SEQ_START << v.x << v.y << v.z SEQ_END;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
{
    out << YAML::Flow;
    out SEQ_START << v.x << v.y << v.z << v.w SEQ_END;
    return out;
}

SceneSerializer::SceneSerializer(const ref<Scene>& p_scene) : mp_scene(p_scene)
{
}

void SceneSerializer::serialize(const std::string& filepath)
{
    NM_UNUSED(filepath);

    YAML::Emitter out;

    out MAP_START;  // scene
    out KEYVAL("scene", mp_scene->m_name);
    out KEYVAL("entities", YAML::BeginSeq);
    for (auto [entityHandle] :
         mp_scene->m_registry.storage<entt::entity>().each())
    {
        Entity entity = {entityHandle, mp_scene.get()};
        if (!entity)  // ?
        {
            return;
        }

        _serializeEntity(out, entity);
    }

    out SEQ_END;
    out MAP_END;  // scene

    std::filesystem::path path(filepath);
    std::ofstream         fout(path);

    fout << out.c_str();
}
void SceneSerializer::serializeBin(const std::string& filepath)
{
    NM_UNUSED(filepath);
    NM_CORE_ASSERT(false, "NOT IMPLEMENTED");
}

bool SceneSerializer::deserialize(const std::string& filepath)
{
    NM_UNUSED(filepath);

    return false;
}

bool SceneSerializer::deserializeBin(const std::string& filepath)
{
    NM_UNUSED(filepath);
    NM_CORE_ASSERT(false, "NOT IMPLEMENTED");

    return false;

}

////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////
void SceneSerializer::_serializeEntity(YAML::Emitter& out, Entity entity)
{
    out MAP_START; // entity
    out KEY ("entity") VAL("1341451951951895"); // TODO UUID goes here;
    
    if(entity.hasComponent<NameCmp>())
    {
        out KEY("nameCmp");
        out MAP_START; // name

        auto& name = entity.getComponent<NameCmp>().name;
        out KEYVAL("name", name);

        out MAP_END; // name

    }

    if(entity.hasComponent<TransformCmp>())
    {
        out KEY("transformCmp");
        out MAP_START; // transform

        auto& transform = entity.getComponent<TransformCmp>();

        out KEYVAL("translation", transform.getTranslation());
        out KEYVAL("rotation", transform.getTranslation());
        out KEYVAL("scale", transform.getTranslation());
        
        out MAP_END; // transform

    }

    
    if(entity.hasComponent<CameraCmp>())
    {
        out KEY("CameraCmp");
        out MAP_START; // camera

        auto& camera = entity.getComponent<CameraCmp>();

        out KEYVAL("primary", camera.primary);
        out KEYVAL("fixedAspect", camera.fixedAspect);
        out KEYVAL("type", (int)camera.camera.getType());
        out KEYVAL("aspectRatio", camera.camera.getAspectRatio());
        out KEYVAL("position", camera.camera.getPosition());
        out KEYVAL("yaw", camera.camera.getYaw());
        out KEYVAL("pitch", camera.camera.getPitch());
        out KEYVAL("speed", camera.camera.getSpeed());
        out KEYVAL("sensitivity", camera.camera.getSensitivity());
        out KEYVAL("zoom", camera.camera.getZoom());
        out KEYVAL("fov", camera.camera.getFov());
        out KEYVAL("farClip", camera.camera.getFarClip());
        out KEYVAL("nearClip", camera.camera.getNearClip());

        out MAP_END;  // transform
    }
    
    
    
    out MAP_END; // entity

}

}  // namespace nimbus