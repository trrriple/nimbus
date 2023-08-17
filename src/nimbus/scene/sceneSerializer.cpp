#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/scene/sceneSerializer.hpp"
#include "nimbus/scene/scene.hpp"
#include "nimbus/scene/entity.hpp"
#include "nimbus/scene/component.hpp"
#include "nimbus/core/resourceManager.hpp"
#include "nimbus/core/application.hpp"

#define TOML_EXCEPTIONS 0
#include "toml++/toml.h"

#include <fstream>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////
// YAML Decoding type overloads
////////////////////////////////////////////////////////////////////////////////
namespace nimbus
{

static void s_serializeEntity(toml::table& entitiesTbl,
                              Entity       entity,
                              GuidCmp&     guidCmp)
{
    toml::table entityTbl;
    entityTbl.insert("sequenceIndex", guidCmp.sequenceIndex);

    ///////////////////////////
    // NameCmp
    ///////////////////////////
    NM_CORE_ASSERT_STATIC(entity.hasComponent<NameCmp>(),
                          "Entity needs NameCmp to serialize!");

    entityTbl.insert("name", entity.getComponent<NameCmp>().name);

    ///////////////////////////
    // TransformCmp
    ///////////////////////////
    if (entity.hasComponent<TransformCmp>())
    {
        toml::table   transformTbl;
        TransformCmp& tc = entity.getComponent<TransformCmp>();

        auto& translation = tc.world.getTranslation();
        auto& rotation    = tc.world.getRotation();
        auto& scale       = tc.world.getScale();

        transformTbl.insert(
            "translation",
            toml::array{translation.x, translation.y, translation.z});

        transformTbl.insert("rotation",
                            toml::array{rotation.x, rotation.y, rotation.z});

        transformTbl.insert("scale", toml::array{scale.x, scale.y, scale.z});
        transformTbl.insert("scaleLocked", tc.world.isScaleLocked());

        entityTbl.insert("TransformCmp", transformTbl);
    }

    ///////////////////////////
    // SpriteCmp
    ///////////////////////////
    if (entity.hasComponent<SpriteCmp>())
    {
        toml::table spriteTbl;
        SpriteCmp&  sc = entity.getComponent<SpriteCmp>();

        spriteTbl.insert(
            "color",
            toml::array{sc.color.r, sc.color.g, sc.color.b, sc.color.a});

        if (sc.p_texture != nullptr)
        {
            toml::table textureTbl;
            textureTbl.insert("path", sc.p_texture->getPath());
            textureTbl.insert("tilingFactor", sc.tilingFactor);
            spriteTbl.insert("texture", textureTbl);
        }

        entityTbl.insert("SpriteCmp", spriteTbl);
    }

    ///////////////////////////
    // TextCmp
    ///////////////////////////
    if (entity.hasComponent<TextCmp>())
    {
        toml::table textTbl;
        TextCmp&    tc = entity.getComponent<TextCmp>();

        textTbl.insert("text", tc.text);

        toml::table formatTbl;
        if (tc.format.p_font != nullptr)
        {
            formatTbl.insert("path", tc.format.p_font->getPath());
        }

        formatTbl.insert("fgColor",
                         toml ::array{tc.format.fgColor.r,
                                      tc.format.fgColor.g,
                                      tc.format.fgColor.b,
                                      tc.format.fgColor.a});

        formatTbl.insert("bgColor",
                         toml ::array{tc.format.bgColor.r,
                                      tc.format.bgColor.g,
                                      tc.format.bgColor.b,
                                      tc.format.bgColor.a});

        formatTbl.insert("kerning", tc.format.kerning);
        formatTbl.insert("leading", tc.format.leading);

        textTbl.insert("format", formatTbl);
        entityTbl.insert("TextCmp", textTbl);
    }

    ///////////////////////////
    // Camera
    ///////////////////////////
    if (entity.hasComponent<CameraCmp>())
    {   
        toml::table camTbl;

        auto& camera = entity.getComponent<CameraCmp>();

        camTbl.insert("primary", camera.primary);
        camTbl.insert("fixedAspect", camera.fixedAspect);
        camTbl.insert("type", (int)camera.camera.getType());
        camTbl.insert("aspectRatio", camera.camera.getAspectRatio());

        glm::vec3 camPos = camera.camera.getPosition();

        camTbl.insert("position", toml::array(camPos.x, camPos.y, camPos.z));
        camTbl.insert("yaw", camera.camera.getYaw());
        camTbl.insert("pitch", camera.camera.getPitch());
        camTbl.insert("speed", camera.camera.getSpeed());
        camTbl.insert("sensitivity", camera.camera.getSensitivity());
        camTbl.insert("zoom", camera.camera.getZoom());
        camTbl.insert("fov", camera.camera.getFov());
        camTbl.insert("farClip", camera.camera.getFarClip());
        camTbl.insert("nearClip", camera.camera.getNearClip());

        entityTbl.insert("CameraCmp", camTbl);
    }

    entitiesTbl.insert(guidCmp.guid.toString(), entityTbl);
}

////////////////////////////////////////////////////////////////////////////////
// YAML Encoding type overloads
////////////////////////////////////////////////////////////////////////////////
SceneSerializer::SceneSerializer(const ref<Scene>& p_scene) : mp_scene(p_scene)
{
}

void SceneSerializer::serialize(const std::string& filepath)
{
    toml::table sceneTbl;
    sceneTbl.insert("Scene", mp_scene->m_name);
    toml::table entitiesTbl;

    mp_scene->sortEntities();
    auto entities = mp_scene->m_registry.view<GuidCmp>();

    for (auto [entityHandle, guid] : entities.each())
    {
        Entity entity = {entityHandle, mp_scene.raw()};
        if (!entity)  // ?
        {
            return;
        }

        s_serializeEntity(entitiesTbl, entity, guid);
    }

    sceneTbl.insert("Entities", entitiesTbl);
    std::filesystem::path path(filepath);
    std::ofstream         fout(path);

    fout << sceneTbl;
}
void SceneSerializer::serializeBin(const std::string& filepath)
{
    NM_UNUSED(filepath);
    NM_CORE_ASSERT(false, "NOT IMPLEMENTED");
}

void SceneSerializer::_deserializeEntity(void*             entityTbl,
                                         Scene*            p_scene,
                                         const std::string guidStr)
{
    toml::table* p_entityTbl = reinterpret_cast<toml::table*>(entityTbl);

    uint32_t     sequenceIndex = (*p_entityTbl)["sequenceIndex"].value_or(0);
    std::string& name         = (*p_entityTbl)["name"].ref<std::string>();

    Log::coreTrace("Name %s, guidStr %s, sequenceIndex %i",
                   name.c_str(),
                   guidStr.c_str(),
                   sequenceIndex);

    Entity entity = p_scene->_addEntity(name, guidStr, sequenceIndex);

    p_entityTbl->for_each(
        [&](const toml::key& key, toml::node& node)
        {
            std::string cmpType(key.data());

            Log::coreInfo("Component Type %s", cmpType.c_str());

            if (node.is_table())
            {
                auto& cmpTbl = *node.as_table();

                ///////////////////////////
                // TransformCmp
                ///////////////////////////
                if (cmpType == "TransformCmp")
                {
                    auto& tc = entity.addComponent<TransformCmp>();

                    auto& translation = *cmpTbl["translation"].as_array();
                    auto& rotation    = *cmpTbl["rotation"].as_array();
                    auto& scale       = *cmpTbl["scale"].as_array();

                    tc.world.setTranslation({translation[0].ref<double>(),
                                                 translation[1].ref<double>(),
                                                 translation[2].ref<double>()});

                    tc.world.setRotation({rotation[0].ref<double>(),
                                              rotation[1].ref<double>(),
                                              rotation[2].ref<double>()});

                    tc.world.setScale({scale[0].ref<double>(),
                                           scale[1].ref<double>(),
                                           scale[2].ref<double>()});

                    tc.world.setScaleLocked(
                        cmpTbl["scaleLocked"].ref<bool>());
                }

                ///////////////////////////
                // SpriteCmp
                ///////////////////////////
                if (cmpType == "SpriteCmp")
                {
                    auto& sc = entity.addComponent<SpriteCmp>();

                    auto& color = *cmpTbl["color"].as_array();

                    sc.color = glm::vec4(color[0].ref<double>(),
                                         color[1].ref<double>(),
                                         color[2].ref<double>(),
                                         color[3].ref<double>());

                    auto texture = cmpTbl["texture"].as_table();

                    if (texture)
                    {
                        sc.p_texture
                            = Application::s_get()
                                  .getResourceManager()
                                  .loadTexture(
                                      Texture::Type::DIFFUSE,
                                      (*texture)["path"].ref<std::string>());

                        sc.tilingFactor
                            = (*texture)["tilingFactor"].ref<double>();
                    }
                }

                ///////////////////////////
                // TextCmp
                ///////////////////////////
                if (cmpType == "TextCmp")
                {
                    auto& tc = entity.addComponent<TextCmp>();

                    tc.text = cmpTbl["text"].ref<std::string>();

                    auto formatTbl = cmpTbl["format"].as_table();

                    if (formatTbl)
                    {
                        auto fontPath
                            = (*formatTbl)["path"].value<std::string>();
                        if (fontPath)
                        {
                            tc.format.p_font = Application::s_get()
                                                   .getResourceManager()
                                                   .loadFont(fontPath.value());
                        }

                        auto fgColor = (*formatTbl)["fgColor"].as_array();

                        tc.format.fgColor
                            = glm::vec4((*fgColor)[0].ref<double>(),
                                        (*fgColor)[1].ref<double>(),
                                        (*fgColor)[2].ref<double>(),
                                        (*fgColor)[3].ref<double>());

                        auto bgColor = (*formatTbl)["bgColor"].as_array();

                        tc.format.bgColor
                            = glm::vec4((*bgColor)[0].ref<double>(),
                                        (*bgColor)[1].ref<double>(),
                                        (*bgColor)[2].ref<double>(),
                                        (*bgColor)[3].ref<double>());

                        tc.format.kerning
                            = (*formatTbl)["kerning"].ref<double>();
                        tc.format.leading
                            = (*formatTbl)["leading"].ref<double>();
                    }
                }

                ///////////////////////////
                // Camera
                ///////////////////////////
                if (cmpType == "CameraCmp")
                {
                    auto& cc = entity.addComponent<CameraCmp>();

                    cc.primary     = cmpTbl["primary"].ref<bool>();
                    cc.fixedAspect = cmpTbl["fixedAspect"].ref<bool>();

                    cc.camera.setType(static_cast<Camera::Type>(
                        cmpTbl["type"].ref<int64_t>()));

                    cc.camera.setAspectRatio(
                        cmpTbl["aspectRatio"].ref<double>());

                    auto position = cmpTbl["position"].as_array();

                    cc.camera.setPosition({(*position)[0].ref<double>(),
                                           (*position)[1].ref<double>(),
                                           (*position)[2].ref<double>()});

                    cc.camera.setYaw(cmpTbl["yaw"].ref<double>());
                    cc.camera.setPitch(cmpTbl["pitch"].ref<double>());
                    cc.camera.setSpeed(cmpTbl["speed"].ref<double>());
                    cc.camera.setSensitivity(
                        cmpTbl["sensitivity"].ref<double>());
                    cc.camera.setZoom(cmpTbl["zoom"].ref<double>());
                    cc.camera.setFov(cmpTbl["fov"].ref<double>());
                    cc.camera.setFarClip(cmpTbl["farClip"].ref<double>());
                    cc.camera.setNearClip(cmpTbl["nearClip"].ref<double>());
                }
            }
        });
}

bool SceneSerializer::deserialize(const std::string& filepath)
{
    auto result = toml::parse_file(filepath);

    if (!result)
    {
        std::ostringstream oss;

        oss << "Error parsing file '" << *result.error().source().path << "':\n"
            << result.error().description() << "\n  ("
            << result.error().source().begin << ")\n";

        std::string errorMessage = oss.str();

        Log::coreError("%s", errorMessage.c_str());
        return false;
    }

    toml::table sceneTbl = std::move(result).table();

    std::optional<std::string> sceneNm = sceneTbl["Scene"].value<std::string>();
    if (!sceneNm)
    {
        Log::coreError("Scene tag missing from file %s", filepath.c_str());

        return false;
    }

    Log::coreInfo("Deserialzing scene %s", sceneNm.value().c_str());

    auto entitiesTbl = sceneTbl["Entities"].as_table();

    if (!entitiesTbl)
    {
        Log::coreError("Entities table missing from file %s", filepath.c_str());

        return false;
    }

    entitiesTbl->for_each(
        [this](const toml::key& key, auto&& entityTbl)
        {
            std::string guidStr(key.data());

            if constexpr (toml::is_table<decltype(entityTbl)>)
            {
                _deserializeEntity((void*)&entityTbl, mp_scene.raw(), guidStr);
            }
        });

    mp_scene->sortEntities();
    return true;
}

bool SceneSerializer::deserializeBin(const std::string& filepath)
{
    NM_UNUSED(filepath);
    NM_CORE_ASSERT(false, "NOT IMPLEMENTED");

    return false;
}

}  // namespace nimbus