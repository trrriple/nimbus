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
#define KEY(x) << YAML::Key << x
#define VAL(x) << YAML::Value << x
#define KEYVAL(x, y) KEY(x) VAL(y)
#define MAP_START << YAML::BeginMap
#define MAP_END << YAML::EndMap
#define SEQ_START << YAML::BeginSeq
#define SEQ_END << YAML::EndSeq

static const std::string k_notAvailable = "na";

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

static void s_serializeEntity(YAML::Emitter& out, Entity entity)
{
    out MAP_START;                              // entity
    out KEY("entity") VAL("1341451951951895");  // TODO UUID goes here;

    if (entity.hasComponent<NameCmp>())
    {
        out KEY("NameCmp");
        out MAP_START;  // name

        auto& name = entity.getComponent<NameCmp>().name;
        out   KEYVAL("name", name);

        out MAP_END;  // name
    }

    if (entity.hasComponent<TransformCmp>())
    {
        out KEY("TransformCmp");
        out MAP_START;  // transform

        auto& transform = entity.getComponent<TransformCmp>();

        out KEYVAL("translation", transform.getTranslation());
        out KEYVAL("rotation", transform.getTranslation());
        out KEYVAL("scale", transform.getTranslation());

        out MAP_END;  // transform
    }

    if (entity.hasComponent<SpriteCmp>())
    {
        out   KEY("SpriteCmp");
        out   MAP_START;  // sprite
        auto& sprite = entity.getComponent<SpriteCmp>();

        out KEYVAL("color", sprite.color);
        if (sprite.p_texture != nullptr)
        {
            out KEY("texture");
            out MAP_START;  // texture
            out KEYVAL("path", sprite.p_texture->getPath());
            out KEYVAL("tilingFactor", sprite.tilingFactor);
            out MAP_END;  // texture
        }

        out MAP_END;  // sprite
    }

    if (entity.hasComponent<TextCmp>())
    {
        out   KEY("TextCmp");
        out   MAP_START;  // text
        auto& text = entity.getComponent<TextCmp>();

        out KEYVAL("text", text.text);

        out KEY("format");
        out MAP_START;  // format
        if (text.format.p_font != nullptr)
        {
            out KEYVAL("path", text.format.p_font->getPath());
        }
        else
        {
            out KEYVAL("path", k_notAvailable);
        }
        out KEYVAL("fgColor", text.format.fgColor);
        out KEYVAL("bgColor", text.format.bgColor);
        out KEYVAL("kerning", text.format.kerning);
        out KEYVAL("leading", text.format.leading);
        out MAP_END;  // format
        out MAP_END;  // text
    }

    if (entity.hasComponent<CameraCmp>())
    {
        out KEY("CameraCmp");
        out MAP_START;  // camera

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

    out MAP_END;  // entity
}
static void s_deserializeEntity(YAML::detail::iterator_value node,
                                Scene*                       p_scene)
{
   
}

SceneSerializer::SceneSerializer(const ref<Scene>& p_scene) : mp_scene(p_scene)
{
}

void SceneSerializer::serialize(const std::string& filepath)
{
    NM_UNUSED(filepath);

    YAML::Emitter out;

    out MAP_START;  // scene
    out KEYVAL("Scene", mp_scene->m_name);
    out KEYVAL("Entities", YAML::BeginSeq);
    for (auto [entityHandle] :
         mp_scene->m_registry.storage<entt::entity>().each())
    {
        Entity entity = {entityHandle, mp_scene.get()};
        if (!entity)  // ?
        {
            return;
        }

        s_serializeEntity(out, entity);
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
    std::ifstream stream(filepath);

    std::stringstream strStream;

    strStream << stream.rdbuf();

    YAML::Node data = YAML::Load(strStream.str());
    if (!data["Scene"])
    {
        Log::coreCritical("'Scene' not present in file!");
        return false;
    }

    std::string sceneName = data["Scene"].as<std::string>();

    Log::coreInfo("Deserialzing scene %s", sceneName.c_str());

    auto entities = data["Entities"];

    for (auto entity : entities)
    {
        // s_deserializeEntity(entity, mp_scene.get());

        uint64_t uuid = entity["Entity"].as<uint64_t)(); // TODO

        if (entity["NameCmp"])
        {
            Log::coreInfo("NameCmp: %s",
                          entity["NameCmp"].as<std::string>().c_str());
        }
    }

    return false;
}

bool SceneSerializer::deserializeBin(const std::string& filepath)
{
    NM_UNUSED(filepath);
    NM_CORE_ASSERT(false, "NOT IMPLEMENTED");

    return false;
}


}  // namespace nimbus