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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// YAML Decoding type overloads
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace nimbus
{

static void s_serializeEntity(toml::table& entitiesTbl, Entity entity, GuidCmp& guidCmp)
{
    toml::table entityTbl;
    entityTbl.insert("sequenceIndex", guidCmp.sequenceIndex);

    ///////////////////////////
    // NameCmp
    ///////////////////////////
    NB_CORE_ASSERT_STATIC(entity.hasComponent<NameCmp>(), "Entity needs NameCmp to serialize!");

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
    // ScriptCmp
    ///////////////////////////
    if (entity.hasComponent<ScriptCmp>())
    {
        toml::table scriptTbl;
        ScriptCmp&  sc = entity.getComponent<ScriptCmp>();

        scriptTbl.insert("scriptEntityName", sc.scriptEntityName);

        entityTbl.insert("ScriptCmp", scriptTbl);
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
    // ParticleEmitterCmp
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

        paramTbl.insert("centerPosition",
                        toml::array{pe.parameters.centerPosition.x,
                                    pe.parameters.centerPosition.y,
                                    pe.parameters.centerPosition.z});

        paramTbl.insert("spawnVolumeType", static_cast<int>(pe.parameters.spawnVolumeType));
        paramTbl.insert("lifetimeMin_s", pe.parameters.lifetimeMin_s);
        paramTbl.insert("lifetimeMax_s", pe.parameters.lifetimeMax_s);
        paramTbl.insert("initSpeedMin", pe.parameters.initSpeedMin);
        paramTbl.insert("initSpeedMax", pe.parameters.initSpeedMax);

        paramTbl.insert("accelerationMin",
                        toml::array{pe.parameters.accelerationMin.x,
                                    pe.parameters.accelerationMin.y,
                                    pe.parameters.accelerationMin.z});

        paramTbl.insert("accelerationMax",
                        toml::array{pe.parameters.accelerationMax.x,
                                    pe.parameters.accelerationMax.y,
                                    pe.parameters.accelerationMax.z});

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
    // RigidBody2DCmp
    ///////////////////////////
    if (entity.hasComponent<RigidBody2DCmp>())
    {
        toml::table rbcTbl;

        auto& rbc = entity.getComponent<RigidBody2DCmp>();

        // technically part of spec but helpful to see it outside of spec
        rbcTbl.insert("type", static_cast<int>(rbc.spec.type));

        toml::table spec;
        spec.insert("linearVelocity", toml::array{rbc.spec.linearVelocity.x, rbc.spec.linearVelocity.y});
        spec.insert("angularVelocity", rbc.spec.angularVelocity);
        spec.insert("linearDamping", rbc.spec.linearDamping);
        spec.insert("angularDamping", rbc.spec.angularDamping);
        spec.insert("allowSleep", rbc.spec.allowSleep);
        spec.insert("awake", rbc.spec.awake);
        spec.insert("bullet", rbc.spec.bullet);
        spec.insert("enabled", rbc.spec.enabled);
        spec.insert("gravityScale", rbc.spec.gravityScale);
        rbcTbl.insert("spec", spec);

        Physics2D::ShapeType shapeType = Physics2D::ShapeType::none;
        if (rbc.fixSpec.shape != nullptr)
        {
            shapeType = rbc.fixSpec.shape->type;
        }

        toml::table fixSpec;
        fixSpec.insert("shape", static_cast<int>(shapeType));
        fixSpec.insert("friction", rbc.fixSpec.friction);
        fixSpec.insert("restitution", rbc.fixSpec.restitution);
        fixSpec.insert("restitutionThreshold", rbc.fixSpec.restitutionThreshold);
        fixSpec.insert("density", rbc.fixSpec.density);
        fixSpec.insert("isSensor", rbc.fixSpec.isSensor);
        rbcTbl.insert("fixSpec", fixSpec);

        entityTbl.insert("RigidBody2DCmp", rbcTbl);
    }

    ///////////////////////////
    // CameraCmp
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// YAML Encoding type overloads
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SceneSerializer::SceneSerializer(const ref<Scene>& p_scene) : mp_scene(p_scene)
{
}

void SceneSerializer::serialize(const std::string& filepath)
{
    toml::table sceneTbl;
    sceneTbl.insert("Scene", mp_scene->m_name);
    sceneTbl.insert("scriptAssemblyPath", mp_scene->m_scriptAssemblyPath.generic_wstring());
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
    NB_UNUSED(filepath);
    NB_CORE_ASSERT(false, "NOT IMPLEMENTED");
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
            {translation[0].ref<f64_t>(), translation[1].ref<f64_t>(), translation[2].ref<f64_t>()});

        tc.local.setRotation({rotation[0].ref<f64_t>(), rotation[1].ref<f64_t>(), rotation[2].ref<f64_t>()});

        tc.local.setScale({scale[0].ref<f64_t>(), scale[1].ref<f64_t>(), scale[2].ref<f64_t>()});

        tc.local.setScaleLocked(cmpTbl["scaleLocked"].ref<bool>());
    }

    ///////////////////////////
    // ScriptCmp
    ///////////////////////////
    if (cmpType == "ScriptCmp")
    {
        auto& sc = entity.addComponent<ScriptCmp>();

        sc.scriptEntityName = cmpTbl["scriptEntityName"].ref<std::string>();
    }

    ///////////////////////////
    // SpriteCmp
    ///////////////////////////
    if (cmpType == "SpriteCmp")
    {
        auto& sc = entity.addComponent<SpriteCmp>();

        auto& color = *cmpTbl["color"].as_array();

        sc.color
            = glm::vec4(color[0].ref<f64_t>(), color[1].ref<f64_t>(), color[2].ref<f64_t>(), color[3].ref<f64_t>());

        auto texture = cmpTbl["texture"].as_table();

        if (texture)
        {
            sc.p_texture = Application::s_get().getResourceManager().loadTexture(Texture::Type::diffuse,
                                                                                 (*texture)["path"].ref<std::string>());

            sc.tilingFactor = (*texture)["tilingFactor"].ref<f64_t>();
        }
    }

    ///////////////////////////
    // TextCmp
    ///////////////////////////
    if (cmpType == "TextCmp")
    {
        auto& tc       = entity.addComponent<TextCmp>();
        tc.text        = cmpTbl["text"].ref<std::string>();
        auto formatTbl = cmpTbl["format"].as_table();

        if (formatTbl)
        {
            auto fontPath = (*formatTbl)["path"].value<std::string>();
            if (fontPath)
            {
                tc.format.p_font = Application::s_get().getResourceManager().loadFont(fontPath.value());
            }

            auto fgColor = (*formatTbl)["fgColor"].as_array();

            tc.format.fgColor = glm::vec4((*fgColor)[0].ref<f64_t>(),
                                          (*fgColor)[1].ref<f64_t>(),
                                          (*fgColor)[2].ref<f64_t>(),
                                          (*fgColor)[3].ref<f64_t>());

            auto bgColor = (*formatTbl)["bgColor"].as_array();

            tc.format.bgColor = glm::vec4((*bgColor)[0].ref<f64_t>(),
                                          (*bgColor)[1].ref<f64_t>(),
                                          (*bgColor)[2].ref<f64_t>(),
                                          (*bgColor)[3].ref<f64_t>());

            tc.format.kerning = (*formatTbl)["kerning"].ref<f64_t>();
            tc.format.leading = (*formatTbl)["leading"].ref<f64_t>();
        }
    }

    ///////////////////////////
    // ParticleEmitterCmp
    ///////////////////////////
    if (cmpType == "ParticleEmitterCmp")
    {
        auto& pe = entity.addComponent<ParticleEmitterCmp>();

        pe.numParticles = cmpTbl["numParticles"].ref<i64_t>();

        auto texture = cmpTbl["texture"].as_table();

        if (texture)
        {
            pe.p_texture = Application::s_get().getResourceManager().loadTexture(Texture::Type::diffuse,
                                                                                 (*texture)["path"].ref<std::string>());
        }

        ParticleEmitter::Parameters params;

        auto paramTbl = *cmpTbl["parameters"].as_table();

        params.centerPosition = glm::vec3(paramTbl["centerPosition"][0].ref<f64_t>(),
                                          paramTbl["centerPosition"][1].ref<f64_t>(),
                                          paramTbl["centerPosition"][2].ref<f64_t>());

        params.spawnVolumeType
            = static_cast<ParticleEmitter::SpawnVolumeType>(paramTbl["spawnVolumeType"].ref<i64_t>());

        params.lifetimeMin_s = paramTbl["lifetimeMin_s"].ref<f64_t>();
        params.lifetimeMax_s = paramTbl["lifetimeMax_s"].ref<f64_t>();
        params.initSpeedMin  = paramTbl["initSpeedMin"].ref<f64_t>();
        params.initSpeedMax  = paramTbl["initSpeedMax"].ref<f64_t>();

        params.accelerationMin = glm::vec3(paramTbl["accelerationMin"][0].ref<f64_t>(),
                                           paramTbl["accelerationMin"][1].ref<f64_t>(),
                                           paramTbl["accelerationMin"][2].ref<f64_t>());

        params.accelerationMax = glm::vec3(paramTbl["accelerationMax"][0].ref<f64_t>(),
                                           paramTbl["accelerationMax"][1].ref<f64_t>(),
                                           paramTbl["accelerationMax"][2].ref<f64_t>());

        params.initSizeMin
            = glm::vec2(paramTbl["initSizeMin"][0].ref<f64_t>(), paramTbl["initSizeMin"][1].ref<f64_t>());

        params.initSizeMax
            = glm::vec2(paramTbl["initSizeMax"][0].ref<f64_t>(), paramTbl["initSizeMax"][1].ref<f64_t>());

        params.ejectionBaseAngle_rad   = paramTbl["ejectionBaseAngle_rad"].ref<f64_t>();
        params.ejectionSpreadAngle_rad = paramTbl["ejectionSpreadAngle_rad"].ref<f64_t>();

        for (const auto& colorNode : *paramTbl["colors"].as_array())
        {
            auto& colorTbl = *colorNode.as_table();

            ParticleEmitter::colorSpec color;

            color.colorStart = glm::vec4(colorTbl["colorStart"][0].ref<f64_t>(),
                                         colorTbl["colorStart"][1].ref<f64_t>(),
                                         colorTbl["colorStart"][2].ref<f64_t>(),
                                         colorTbl["colorStart"][3].ref<f64_t>());

            color.colorEnd = glm::vec4(colorTbl["colorEnd"][0].ref<f64_t>(),
                                       colorTbl["colorEnd"][1].ref<f64_t>(),
                                       colorTbl["colorEnd"][2].ref<f64_t>(),
                                       colorTbl["colorEnd"][3].ref<f64_t>());

            params.colors.push_back(color);
        }

        params.circleVolumeParams.radius = paramTbl["circleVolumeParams"]["radius"].ref<f64_t>();
        params.rectVolumeParams.width    = paramTbl["rectVolumeParams"]["width"].ref<f64_t>();
        params.rectVolumeParams.height   = paramTbl["rectVolumeParams"]["height"].ref<f64_t>();
        params.lineVolumeParams.length   = paramTbl["lineVolumeParams"]["length"].ref<f64_t>();
        params.persist                   = paramTbl["persist"].as_boolean();
        params.shrink                    = paramTbl["shrink"].as_boolean();

        params.blendingMode = static_cast<GraphicsApi::BlendingMode>(paramTbl["blendingMode"].ref<i64_t>());

        pe.parameters = params;
    }

    ///////////////////////////
    // RigidBody2DCmp
    ///////////////////////////
    if (cmpType == "RigidBody2DCmp")
    {
        auto& rbc = entity.addComponent<RigidBody2DCmp>();

        rbc.spec.type = static_cast<Physics2D::BodyType>(cmpTbl["type"].ref<i64_t>());

        auto specTbl = *cmpTbl["spec"].as_table();

        rbc.spec.linearVelocity
            = glm::vec2(specTbl["linearVelocity"][0].ref<f64_t>(), specTbl["linearVelocity"][1].ref<f64_t>());

        rbc.spec.angularVelocity = specTbl["angularVelocity"].ref<f64_t>();
        rbc.spec.linearDamping   = specTbl["linearDamping"].ref<f64_t>();
        rbc.spec.angularDamping  = specTbl["angularDamping"].ref<f64_t>();
        rbc.spec.allowSleep      = specTbl["allowSleep"].ref<bool>();
        rbc.spec.awake           = specTbl["awake"].ref<bool>();
        rbc.spec.bullet          = specTbl["bullet"].ref<bool>();
        rbc.spec.enabled         = specTbl["enabled"].ref<bool>();
        rbc.spec.gravityScale    = specTbl["gravityScale"].ref<f64_t>();

        auto fixSpecTbl = *cmpTbl["fixSpec"].as_table();

        Physics2D::ShapeType shapeType = static_cast<Physics2D::ShapeType>(fixSpecTbl["shape"].ref<i64_t>());

        if (shapeType == Physics2D::ShapeType::none)
        {
            rbc.fixSpec.shape = nullptr;
        }
        else if (shapeType == Physics2D::ShapeType::rectangle)
        {
            rbc.fixSpec.shape = &rbc.rectShape;
        }
        else if (shapeType == Physics2D::ShapeType::circle)
        {
            rbc.fixSpec.shape = &rbc.circShape;
        }

        rbc.fixSpec.friction             = fixSpecTbl["friction"].ref<f64_t>();
        rbc.fixSpec.restitution          = fixSpecTbl["restitution"].ref<f64_t>();
        rbc.fixSpec.restitutionThreshold = fixSpecTbl["restitutionThreshold"].ref<f64_t>();
        rbc.fixSpec.density              = fixSpecTbl["density"].ref<f64_t>();
        rbc.fixSpec.isSensor             = fixSpecTbl["isSensor"].ref<bool>();
    }

    ///////////////////////////
    // CameraCmp
    ///////////////////////////
    if (cmpType == "CameraCmp")
    {
        auto& cc = entity.addComponent<CameraCmp>();

        cc.primary     = cmpTbl["primary"].ref<bool>();
        cc.fixedAspect = cmpTbl["fixedAspect"].ref<bool>();

        cc.camera.setType(static_cast<Camera::Type>(cmpTbl["type"].ref<i64_t>()));

        cc.camera.setAspectRatio(cmpTbl["aspectRatio"].ref<f64_t>());

        auto position = cmpTbl["position"].as_array();

        cc.camera.setPosition({(*position)[0].ref<f64_t>(), (*position)[1].ref<f64_t>(), (*position)[2].ref<f64_t>()});

        cc.camera.setYaw(cmpTbl["yaw"].ref<f64_t>());
        cc.camera.setPitch(cmpTbl["pitch"].ref<f64_t>());
        cc.camera.setSpeed(cmpTbl["speed"].ref<f64_t>());
        cc.camera.setSensitivity(cmpTbl["sensitivity"].ref<f64_t>());
        cc.camera.setZoom(cmpTbl["zoom"].ref<f64_t>());
        cc.camera.setFov(cmpTbl["fov"].ref<f64_t>());
        cc.camera.setFarClip(cmpTbl["farClip"].ref<f64_t>());
        cc.camera.setNearClip(cmpTbl["nearClip"].ref<f64_t>());
    }
}

void SceneSerializer::_deserializeEntity(void* entityTbl, Scene* p_scene, const std::string guidStr)
{
    toml::table* p_entityTbl = reinterpret_cast<toml::table*>(entityTbl);

    u32_t        sequenceIndex = (*p_entityTbl)["sequenceIndex"].value_or(0);
    std::string& name          = (*p_entityTbl)["name"].ref<std::string>();

    Log::coreTrace("Name %s, guidStr %s, sequenceIndex %i", name.c_str(), guidStr.c_str(), sequenceIndex);

    Entity entity = p_scene->_addEntity(name, guidStr, sequenceIndex);

    // put this into our map for later
    m_entityMap[guidStr] = entity;

    p_entityTbl->for_each(
        [&](const toml::key& key, toml::node& node)
        {
            std::string cmpType(key.data());

            Log::coreTrace("Component Type %s", cmpType.c_str());

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
    mp_scene->m_name = sceneNm.value();

    std::optional<std::wstring> sceneScriptAssemblyPath = sceneTbl["scriptAssemblyPath"].value<std::wstring>();
    if (sceneScriptAssemblyPath)
    {
        mp_scene->setScriptAssemblyPath(sceneScriptAssemblyPath.value());
    }

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
    NB_UNUSED(filepath);
    NB_CORE_ASSERT(false, "NOT IMPLEMENTED");

    return false;
}

}  // namespace nimbus