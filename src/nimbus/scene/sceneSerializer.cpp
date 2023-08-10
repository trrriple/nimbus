    #include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/scene/sceneSerializer.hpp"
#include "nimbus/scene/scene.hpp"
#include "nimbus/scene/entity.hpp"
#include "nimbus/scene/component.hpp"
#include "nimbus/core/resourceManager.hpp"
#include "nimbus/core/application.hpp"


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

////////////////////////////////////////////////////////////////////////////////
// YAML Decoding type overloads
////////////////////////////////////////////////////////////////////////////////
namespace YAML
{

template <>
struct convert<glm::vec2>
{
    static Node encode(const glm::vec2& rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }

    static bool decode(const Node& node, glm::vec2& rhs)
    {
        if (!node.IsSequence() || node.size() != 2)
            return false;

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        return true;
    }
};

template <>
struct convert<glm::vec3>
{
    static Node encode(const glm::vec3& rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }

    static bool decode(const Node& node, glm::vec3& rhs)
    {
        if (!node.IsSequence() || node.size() != 3)
            return false;

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        return true;
    }
};

template <>
struct convert<glm::vec4>
{
    static Node encode(const glm::vec4& rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        node.push_back(rhs.w);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }

    static bool decode(const Node& node, glm::vec4& rhs)
    {
        if (!node.IsSequence() || node.size() != 4)
            return false;

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        rhs.w = node[3].as<float>();
        return true;
    }
};
}  // namespace YAML

namespace nimbus
{

////////////////////////////////////////////////////////////////////////////////
// YAML Encoding type overloads
////////////////////////////////////////////////////////////////////////////////
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
    out KEY("Entity") VAL("1341451951951895");  // TODO UUID goes here;

    ///////////////////////////
    // NameCmp
    ///////////////////////////
    if (entity.hasComponent<NameCmp>())
    {
        out KEY("NameCmp");
        out MAP_START;  // name

        auto& name = entity.getComponent<NameCmp>().name;
        out   KEYVAL("name", name);

        out MAP_END;  // name
    }

    ///////////////////////////
    // TransformCmp
    ///////////////////////////
    if (entity.hasComponent<TransformCmp>())
    {
        out KEY("TransformCmp");
        out MAP_START;  // transform

        auto& transform = entity.getComponent<TransformCmp>();

        out KEYVAL("translation", transform.getTranslation());
        out KEYVAL("rotation", transform.getRotation());
        out KEYVAL("scale", transform.getScale());
        out KEYVAL("scaleLocked", transform.isScaleLocked());

        out MAP_END;  // transform`
    }

    ///////////////////////////
    // SpriteCmp
    ///////////////////////////
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

    ///////////////////////////
    // TextCmp
    ///////////////////////////
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
            out KEYVAL("path", YAML::Null);
        }
        out KEYVAL("fgColor", text.format.fgColor);
        out KEYVAL("bgColor", text.format.bgColor);
        out KEYVAL("kerning", text.format.kerning);
        out KEYVAL("leading", text.format.leading);
        out MAP_END;  // format
        out MAP_END;  // text
    }

    ///////////////////////////
    // CameraCmp
    ///////////////////////////
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
static void s_deserializeEntity(YAML::Node entityNode, Scene* p_scene)
{
    uint64_t uuid = entityNode["Entity"].as<uint64_t>();  // TODO

    auto nameCmp = entityNode["NameCmp"];

    NM_CORE_ASSERT_STATIC(
        nameCmp, "Can't deserialize entity node without a name component!");

    std::string entityName = nameCmp["name"].as<std::string>();

    Log::coreTrace("NameCmp: %s", entityName.c_str());
    
    Entity entity = p_scene->addEntity(nameCmp["name"].as<std::string>());

    // add other components

    ///////////////////////////
    // TransformCmp
    ///////////////////////////
    auto tcNode = entityNode["TransformCmp"];
    if(tcNode)
    {
        Log::coreTrace("Has TransformCmp");

        auto& tc =  entity.addComponent<TransformCmp>();

        tc.setTranslation(tcNode["translation"].as<glm::vec3>());
        tc.setRotation(tcNode["rotation"].as<glm::vec3>());
        tc.setScale(tcNode["scale"].as<glm::vec3>());
        tc.setScaleLocked(tcNode["scaleLocked"].as<bool>());
    }


    ///////////////////////////
    // SpriteCmp
    ///////////////////////////
    auto spriteNode = entityNode["SpriteCmp"];
    if(spriteNode)
    {
        Log::coreTrace("Has SpriteCmp");

        auto& sc =  entity.addComponent<SpriteCmp>();

        sc.color = spriteNode["color"].as<glm::vec4>();
        
        auto texNode = spriteNode["texture"];

        if(texNode)
        {
            sc.p_texture = Application::s_get().getResourceManager().loadTexture(
                Texture::Type::DIFFUSE, texNode["path"].as<std::string>());

            sc.tilingFactor = texNode["tilingFactor"].as<float>();
        }
    }

     ///////////////////////////
    // TextCmp
    ///////////////////////////
    auto textNode = entityNode["TextCmp"];
    if (textNode)
    {
        Log::coreTrace("Has TextCmp");

        auto& tc = entity.addComponent<TextCmp>();

        tc.text = textNode["text"].as<std::string>();

        auto formatNode = textNode["format"];

        if (!formatNode["path"].IsNull())
        {
            tc.format.p_font = Application::s_get().getResourceManager().loadFont(
                formatNode["path"].as<std::string>());
        }

        tc.format.fgColor = formatNode["fgColor"].as<glm::vec4>();
        tc.format.bgColor = formatNode["bgColor"].as<glm::vec4>();
        tc.format.kerning = formatNode["kerning"].as<float>();
        tc.format.leading = formatNode["leading"].as<float>();
    }

    ///////////////////////////
    // CameraCmp
    ///////////////////////////
    auto cameraNode = entityNode["CameraCmp"];
    if (cameraNode)
    {
        Log::coreTrace("Has CameraCmp");

        auto& cc = entity.addComponent<CameraCmp>();

        cc.primary     = cameraNode["primary"].as<bool>();
        cc.fixedAspect = cameraNode["fixedAspect"].as<bool>();
        cc.camera.setType(
            static_cast<Camera::Type>(cameraNode["type"].as<int>()));
        cc.camera.setAspectRatio(cameraNode["aspectRatio"].as<float>());
        cc.camera.setPosition(cameraNode["position"].as<glm::vec3>());
        cc.camera.setYaw(cameraNode["yaw"].as<float>());
        cc.camera.setPitch(cameraNode["pitch"].as<float>());
        cc.camera.setSpeed(cameraNode["speed"].as<float>());
        cc.camera.setSensitivity(cameraNode["sensitivity"].as<float>());
        cc.camera.setZoom(cameraNode["zoom"].as<float>());
        cc.camera.setFov(cameraNode["fov"].as<float>());
        cc.camera.setFarClip(cameraNode["farClip"].as<float>());
        cc.camera.setNearClip(cameraNode["nearClip"].as<float>());
    }
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
        Entity entity = {entityHandle, mp_scene.raw()};
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

    Log::coreTrace("Deserialzing scene %s", sceneName.c_str());

    auto entitiesNode = data["Entities"];

    // reverse it to add the entities in the order they were 
    // originally added
    std::vector<YAML::Node> entitiesRev(entitiesNode.begin(),
                                        entitiesNode.end());

    for (auto it = entitiesRev.rbegin(); it != entitiesRev.rend(); ++it)
    {
        s_deserializeEntity(*it, mp_scene.raw());
    }

    return true;
}

bool SceneSerializer::deserializeBin(const std::string& filepath)
{
    NM_UNUSED(filepath);
    NM_CORE_ASSERT(false, "NOT IMPLEMENTED");

    return false;
}


}  // namespace nimbus