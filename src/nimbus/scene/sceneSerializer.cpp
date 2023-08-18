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

static void s_serializeEntity(toml::table& entitiesTbl, Entity entity, GuidCmp& guidCmp)
{
    toml::table entityTbl;
    entityTbl.insert("sequenceIndex", guidCmp.sequenceIndex);

    ///////////////////////////
    // NameCmp
    ///////////////////////////
    NM_CORE_ASSERT_STATIC(entity.hasComponent<NameCmp>(), "Entity needs NameCmp to serialize!");

    entityTbl.insert("name", entity.getComponent<NameCmp>().name);

    ///////////////////////////
    // AncestryCmp
    ///////////////////////////
    if (entity.hasComponent<AncestryCmp>())
    {
        toml::table  ancestryTbl;
        AncestryCmp& ac = entity.getComponent<AncestryCmp>();

        if (ac.parent)
        {
            ancestryTbl.insert("parent", ac.parent.getComponent<GuidCmp>().guid.toString());
        }

        toml::array childGuids;
        for (auto& child : ac.children)
        {
            childGuids.push_back(child.getComponent<GuidCmp>().guid.toString());
        }

        ancestryTbl.insert("children", childGuids);

        entityTbl.insert("AncestryCmp", ancestryTbl);
    }

    ///////////////////////////
    // TransformCmp
    ///////////////////////////
    if (entity.hasComponent<TransformCmp>())
    {
        toml::table   transformTbl;
        TransformCmp& tc = entity.getComponent<TransformCmp>();

        auto& translation = tc.local.getTranslation();
        auto& rotation    = tc.local.getRotation();
        auto& scale       = tc.local.getScale();

        transformTbl.insert("translation", toml::array{translation.x, translation.y, translation.z});

        transformTbl.insert("rotation", toml::array{rotation.x, rotation.y, rotation.z});

        transformTbl.insert("scale", toml::array{scale.x, scale.y, scale.z});
        transformTbl.insert("scaleLocked", tc.local.isScaleLocked());

        entityTbl.insert("TransformCmp", transformTbl);
    }

    ///////////////////////////
    // SpriteCmp
    ///////////////////////////
    if (entity.hasComponent<SpriteCmp>())
    {
        toml::table spriteTbl;
        SpriteCmp&  sc = entity.getComponent<SpriteCmp>();

        spriteTbl.insert("color", toml::array{sc.color.r, sc.color.g, sc.color.b, sc.color.a});

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

        formatTbl.insert(
            "fgColor",
            toml ::array{tc.format.fgColor.r, tc.format.fgColor.g, tc.format.fgColor.b, tc.format.fgColor.a});

        formatTbl.insert(
            "bgColor",
            toml ::array{tc.format.bgColor.r, tc.format.bgColor.g, tc.format.bgColor.b, tc.format.bgColor.a});

        formatTbl.insert("kerning", tc.format.kerning);
        formatTbl.insert("leading", tc.format.leading);

        textTbl.insert("format", formatTbl);
        entityTbl.insert("TextCmp", textTbl);
    }

    ///////////////////////////
    // ParticleEmitter
    ///////////////////////////
    if (entity.hasComponent<ParticleEmitterCmp>())
    {
        toml::table         peTbl;
        ParticleEmitterCmp& pe = entity.getComponent<ParticleEmitterCmp>();

        peTbl.insert("numParticles", pe.numParticles);

        if (pe.p_texture != nullptr)
        {
            toml::table textureTbl;
            textureTbl.insert("path", pe.p_texture->getPath());
            peTbl.insert("texture", textureTbl);
        }

        toml::table paramTbl;

        paramTbl.insert(
            "centerPosition",
            toml::array{
                pe.parameters.centerPosition.x, pe.parameters.centerPosition.y, pe.parameters.centerPosition.z});

        paramTbl.insert("spawnVolumeType", static_cast<int>(pe.parameters.spawnVolumeType));
        paramTbl.insert("lifetimeMin_s", pe.parameters.lifetimeMin_s);
        paramTbl.insert("lifetimeMax_s", pe.parameters.lifetimeMax_s);
        paramTbl.insert("initSpeedMin", pe.parameters.initSpeedMin);
        paramTbl.insert("initSpeedMax", pe.parameters.initSpeedMax);

        paramTbl.insert(
            "accelerationMin",
            toml::array{
                pe.parameters.accelerationMin.x, pe.parameters.accelerationMin.y, pe.parameters.accelerationMin.z});

        paramTbl.insert(
            "accelerationMax",
            toml::array{
                pe.parameters.accelerationMax.x, pe.parameters.accelerationMax.y, pe.parameters.accelerationMax.z});

        paramTbl.insert("initSizeMin", toml::array{pe.parameters.initSizeMin.x, pe.parameters.initSizeMin.y});

        paramTbl.insert("initSizeMax", toml::array{pe.parameters.initSizeMax.x, pe.parameters.initSizeMax.y});

        paramTbl.insert("ejectionBaseAngle_rad", pe.parameters.ejectionBaseAngle_rad);
        paramTbl.insert("ejectionSpreadAngle_rad", pe.parameters.ejectionSpreadAngle_rad);

        toml::array colorsArray;
        for (const auto& color : pe.parameters.colors)
        {
            toml::table colorTbl;
            colorTbl.insert(
                "colorStart",
                toml::array{color.colorStart.r, color.colorStart.g, color.colorStart.b, color.colorStart.a});
            colorTbl.insert("colorEnd",
                            toml::array{color.colorEnd.r, color.colorEnd.g, color.colorEnd.b, color.colorEnd.a});
            colorsArray.push_back(colorTbl);
        }
        paramTbl.insert("colors", colorsArray);

        paramTbl.insert("circleVolumeParams", toml::table{{"radius", pe.parameters.circleVolumeParams.radius}});

        paramTbl.insert("rectVolumeParams",
                        toml::table{{"width", pe.parameters.rectVolumeParams.width},
                                    {"height", pe.parameters.rectVolumeParams.height}});

        paramTbl.insert("lineVolumeParams", toml::table{{"length", pe.parameters.lineVolumeParams.length}});

        paramTbl.insert("persist", pe.parameters.persist);
        paramTbl.insert("shrink", pe.parameters.shrink);
        paramTbl.insert("blendingMode", static_cast<int>(pe.parameters.blendingMode));

        peTbl.insert("parameters", paramTbl);
        entityTbl.insert("ParticleEmitterCmp", peTbl);
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

static void _s_deserializeComponent(Entity entity, const std::string& cmpType, toml::table& cmpTbl)
{
    ///////////////////////////
    // TransformCmp
    ///////////////////////////
    if (cmpType == "TransformCmp")
    {
        auto& tc = entity.addComponent<TransformCmp>();

        auto& translation = *cmpTbl["translation"].as_array();
        auto& rotation    = *cmpTbl["rotation"].as_array();
        auto& scale       = *cmpTbl["scale"].as_array();

        tc.local.setTranslation(
            {translation[0].ref<double>(), translation[1].ref<double>(), translation[2].ref<double>()});

        tc.local.setRotation({rotation[0].ref<double>(), rotation[1].ref<double>(), rotation[2].ref<double>()});

        tc.local.setScale({scale[0].ref<double>(), scale[1].ref<double>(), scale[2].ref<double>()});

        tc.local.setScaleLocked(cmpTbl["scaleLocked"].ref<bool>());
    }

    ///////////////////////////
    // SpriteCmp
    ///////////////////////////
    if (cmpType == "SpriteCmp")
    {
        auto& sc = entity.addComponent<SpriteCmp>();

        auto& color = *cmpTbl["color"].as_array();

        sc.color
            = glm::vec4(color[0].ref<double>(), color[1].ref<double>(), color[2].ref<double>(), color[3].ref<double>());

        auto texture = cmpTbl["texture"].as_table();

        if (texture)
        {
            sc.p_texture = Application::s_get().getResourceManager().loadTexture(Texture::Type::DIFFUSE,
                                                                                 (*texture)["path"].ref<std::string>());

            sc.tilingFactor = (*texture)["tilingFactor"].ref<double>();
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
            auto fontPath = (*formatTbl)["path"].value<std::string>();
            if (fontPath)
            {
                tc.format.p_font = Application::s_get().getResourceManager().loadFont(fontPath.value());
            }

            auto fgColor = (*formatTbl)["fgColor"].as_array();

            tc.format.fgColor = glm::vec4((*fgColor)[0].ref<double>(),
                                          (*fgColor)[1].ref<double>(),
                                          (*fgColor)[2].ref<double>(),
                                          (*fgColor)[3].ref<double>());

            auto bgColor = (*formatTbl)["bgColor"].as_array();

            tc.format.bgColor = glm::vec4((*bgColor)[0].ref<double>(),
                                          (*bgColor)[1].ref<double>(),
                                          (*bgColor)[2].ref<double>(),
                                          (*bgColor)[3].ref<double>());

            tc.format.kerning = (*formatTbl)["kerning"].ref<double>();
            tc.format.leading = (*formatTbl)["leading"].ref<double>();
        }
    }

    ///////////////////////////
    // ParticleEmitterCmp
    ///////////////////////////
    if (cmpType == "ParticleEmitterCmp")
    {
        auto& pe = entity.addComponent<ParticleEmitterCmp>();

        ParticleEmitter::Parameters params;

        params.centerPosition = glm::vec3(cmpTbl["centerPosition"][0].ref<double>(),
                                          cmpTbl["centerPosition"][1].ref<double>(),
                                          cmpTbl["centerPosition"][2].ref<double>());

        params.spawnVolumeType
            = static_cast<ParticleEmitter::SpawnVolumeType>(cmpTbl["spawnVolumeType"].ref<int64_t>());

        params.lifetimeMin_s = cmpTbl["lifetimeMin_s"].ref<double>();
        params.lifetimeMax_s = cmpTbl["lifetimeMax_s"].ref<double>();
        params.initSpeedMin  = cmpTbl["initSpeedMin"].ref<double>();
        params.initSpeedMax  = cmpTbl["initSpeedMax"].ref<double>();

        params.accelerationMin = glm::vec3(cmpTbl["accelerationMin"][0].ref<double>(),
                                           cmpTbl["accelerationMin"][1].ref<double>(),
                                           cmpTbl["accelerationMin"][2].ref<double>());

        params.accelerationMax = glm::vec3(cmpTbl["accelerationMax"][0].ref<double>(),
                                           cmpTbl["accelerationMax"][1].ref<double>(),
                                           cmpTbl["accelerationMax"][2].ref<double>());

        params.initSizeMin = glm::vec2(cmpTbl["initSizeMin"][0].ref<double>(), cmpTbl["initSizeMin"][1].ref<double>());

        params.initSizeMax = glm::vec2(cmpTbl["initSizeMax"][0].ref<double>(), cmpTbl["initSizeMax"][1].ref<double>());

        params.ejectionBaseAngle_rad   = cmpTbl["ejectionBaseAngle_rad"].ref<double>();
        params.ejectionSpreadAngle_rad = cmpTbl["ejectionSpreadAngle_rad"].ref<double>();

        for (const auto& colorNode : *cmpTbl["colors"].as_array())
        {
            auto& colorTbl = *colorNode.as_table();

            ParticleEmitter::colorSpec color;

            color.colorStart = glm::vec4(colorTbl["colorStart"][0].ref<double>(),
                                         colorTbl["colorStart"][1].ref<double>(),
                                         colorTbl["colorStart"][2].ref<double>(),
                                         colorTbl["colorStart"][3].ref<double>());

            color.colorEnd = glm::vec4(colorTbl["colorEnd"][0].ref<double>(),
                                       colorTbl["colorEnd"][1].ref<double>(),
                                       colorTbl["colorEnd"][2].ref<double>(),
                                       colorTbl["colorEnd"][3].ref<double>());

            params.colors.push_back(color);
        }

        params.circleVolumeParams.radius = cmpTbl["circleVolumeParams"]["radius"].ref<double>();

        params.rectVolumeParams.width = cmpTbl["rectVolumeParams"]["width"].ref<double>();

        params.rectVolumeParams.height = cmpTbl["rectVolumeParams"]["height"].ref<double>();

        params.lineVolumeParams.length = cmpTbl["lineVolumeParams"]["length"].ref<double>();

        params.persist = cmpTbl["persist"].as_boolean();
        params.shrink  = cmpTbl["shrink"].as_boolean();

        params.blendingMode = static_cast<GraphicsApi::BlendingMode>(cmpTbl["blendingMode"].ref<int64_t>());

        pe.parameters   = params;
        pe.numParticles = cmpTbl["numParticles"].ref<int64_t>();

        auto texture = cmpTbl["texture"].as_table();

        if (texture)
        {
            pe.p_texture = Application::s_get().getResourceManager().loadTexture(Texture::Type::DIFFUSE,
                                                                                 (*texture)["path"].ref<std::string>());
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

        cc.camera.setType(static_cast<Camera::Type>(cmpTbl["type"].ref<int64_t>()));

        cc.camera.setAspectRatio(cmpTbl["aspectRatio"].ref<double>());

        auto position = cmpTbl["position"].as_array();

        cc.camera.setPosition(
            {(*position)[0].ref<double>(), (*position)[1].ref<double>(), (*position)[2].ref<double>()});

        cc.camera.setYaw(cmpTbl["yaw"].ref<double>());
        cc.camera.setPitch(cmpTbl["pitch"].ref<double>());
        cc.camera.setSpeed(cmpTbl["speed"].ref<double>());
        cc.camera.setSensitivity(cmpTbl["sensitivity"].ref<double>());
        cc.camera.setZoom(cmpTbl["zoom"].ref<double>());
        cc.camera.setFov(cmpTbl["fov"].ref<double>());
        cc.camera.setFarClip(cmpTbl["farClip"].ref<double>());
        cc.camera.setNearClip(cmpTbl["nearClip"].ref<double>());
    }
}

void SceneSerializer::_deserializeEntity(void* entityTbl, Scene* p_scene, const std::string guidStr)
{
    toml::table* p_entityTbl = reinterpret_cast<toml::table*>(entityTbl);

    uint32_t     sequenceIndex = (*p_entityTbl)["sequenceIndex"].value_or(0);
    std::string& name          = (*p_entityTbl)["name"].ref<std::string>();

    Log::coreTrace("Name %s, guidStr %s, sequenceIndex %i", name.c_str(), guidStr.c_str(), sequenceIndex);

    Entity entity = p_scene->_addEntity(name, guidStr, sequenceIndex);

    // put this into our map for later
    m_entityMap[guidStr] = entity;

    p_entityTbl->for_each(
        [&](const toml::key& key, toml::node& node)
        {
            std::string cmpType(key.data());

            Log::coreInfo("Component Type %s", cmpType.c_str());

            if (node.is_table())
            {
                auto& cmpTbl = *node.as_table();

                _s_deserializeComponent(entity, cmpType, cmpTbl);
            }
        });
}

void SceneSerializer::_assembleFamilyTree(void* entityTbl, const std::string guidStr)
{
    toml::table* p_entityTbl = reinterpret_cast<toml::table*>(entityTbl);

    Entity entity = m_entityMap[guidStr];

    p_entityTbl->for_each(
        [&](const toml::key& key, toml::node& node)
        {
            std::string cmpType(key.data());

            if (node.is_table())
            {
                auto& cmpTbl = *node.as_table();

                ///////////////////////////
                // AncestryCmp
                ///////////////////////////
                if (cmpType == "AncestryCmp")
                {
                    auto& ac = entity.addComponent<AncestryCmp>();

                    std::optional<std::string> parentGuidStr = cmpTbl["parent"].value<std::string>();

                    if (parentGuidStr)
                    {
                        ac.parent = m_entityMap[parentGuidStr.value()];
                    }

                    auto childrenGuidStrs = cmpTbl["children"].as_array();
                    if (childrenGuidStrs)
                    {
                        for (auto& childGuidStr : *childrenGuidStrs)
                        {
                            ac.children.push_back(m_entityMap[childGuidStr.ref<std::string>()]);
                        }
                    }
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
            << result.error().description() << "\n  (" << result.error().source().begin << ")\n";

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

    // go through again, but this time do stuff we couldn't do without
    // the full picture
    entitiesTbl->for_each(
        [this](const toml::key& key, auto&& entityTbl)
        {
            std::string guidStr(key.data());

            if constexpr (toml::is_table<decltype(entityTbl)>)
            {
                _assembleFamilyTree((void*)&entityTbl, guidStr);
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