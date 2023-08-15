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
    entityTbl.insert("genesisIndex", guidCmp.genesisIndex);


    ///////////////////////////
    // NameCmp
    ///////////////////////////
    if (entity.hasComponent<NameCmp>())
    {
        toml::table nameTbl;
        nameTbl.insert("name", entity.getComponent<NameCmp>().name);
        entityTbl.insert("NameCmp", nameTbl);
    }

    ///////////////////////////
    // TransformCmp
    ///////////////////////////
    if (entity.hasComponent<TransformCmp>())
    {
        toml::table transformTbl;
        TransformCmp& transformCmp = entity.getComponent<TransformCmp>();

        auto& translation = transformCmp.getTranslation();
        auto& rotation    = transformCmp.getRotation();
        auto& scale       = transformCmp.getScale();

        transformTbl.insert(
            "translation",
            toml::array{translation.x, translation.y, translation.z});

        transformTbl.insert("rotation",
                            toml::array{rotation.x, rotation.y, rotation.z});

        transformTbl.insert("scale", toml::array{scale.x, scale.y, scale.z});
        transformTbl.insert("scaleLocked", transformCmp.isScaleLocked());

        entityTbl.insert("TransformCmp", transformTbl);
    }


    ///////////////////////////
    // SpriteCmp
    ///////////////////////////
    if (entity.hasComponent<SpriteCmp>())
    {
        toml::table spriteTbl;
        SpriteCmp&  spriteCmp = entity.getComponent<SpriteCmp>();

        spriteTbl.insert("color",
                         toml::array{spriteCmp.color.r,
                                     spriteCmp.color.g,
                                     spriteCmp.color.b,
                                     spriteCmp.color.a});

        if (spriteCmp.p_texture != nullptr)
        {
            toml::table textureTbl;
            textureTbl.insert("path", spriteCmp.p_texture->getPath());
            textureTbl.insert("tilingFactor", spriteCmp.tilingFactor);
            spriteTbl.insert("texture", textureTbl);
        }

        entityTbl.insert("SpriteCmp", spriteTbl);
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
    sceneTbl.insert("scene", mp_scene->m_name);
    toml::table entitiesTbl;
    
    mp_scene->sortEntities();
    auto entities = mp_scene->m_registry.view<GuidCmp>();

    for (auto [entityHandle, guid] : entities.each())
    {
        Log::coreInfo("%i", guid.genesisIndex);

        Entity entity = {entityHandle, mp_scene.raw()};
        if (!entity)  // ?
        {
            return;
        }

        s_serializeEntity(entitiesTbl, entity, guid);
    }

    sceneTbl.insert("entities", entitiesTbl);
    std::filesystem::path path(filepath);
    std::ofstream         fout(path);

    fout << sceneTbl;
}
void SceneSerializer::serializeBin(const std::string& filepath)
{
    NM_UNUSED(filepath);
    NM_CORE_ASSERT(false, "NOT IMPLEMENTED");
}

static void s_deserializeEntity(toml::table       entityTbl,
                                Scene*            p_scene,
                                const std::string guidStr)
{
    uint32_t genesisIndex = entityTbl["genesisIndex"].value_or(0);

    Log::coreInfo("guidStr %s, GenesisIndex %i", guidStr.c_str(), genesisIndex);

    entityTbl.for_each(
        [p_scene](const toml::key& key, auto&& cmpTbl)
        {
            std::string cmpType(key.data());

            Log::coreInfo("Component Type %s", cmpType.c_str());

            if constexpr (toml::is_table<decltype(cmpTbl)>)
            {
                if(cmpType == "NameCmp")
                {
                    std::optional<std::string> name
                        = cmpTbl["name"].template value<std::string>();
                    
                    if(name)
                    {
                        Log::coreInfo("Name %s", name.value().c_str());

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
            << result.error().description() << "\n  ("
            << result.error().source().begin << ")\n";

        std::string errorMessage = oss.str();

        Log::coreError("%s", errorMessage.c_str());
        return false;
    }

    toml::table sceneTbl = std::move(result).table();

    std::optional<std::string> sceneNm = sceneTbl["scene"].value<std::string>();
    if(!sceneNm)
    {
        Log::coreError("Scene tag missing from file %s", filepath.c_str());

        return false;
    }

    Log::coreInfo("Deserialzing scene %s", sceneNm.value().c_str());


    auto entitiesTbl = sceneTbl["entities"].as_table();

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
                s_deserializeEntity(entityTbl, mp_scene.raw(), guidStr);
            }

        });

    return true;
}

bool SceneSerializer::deserializeBin(const std::string& filepath)
{
    NM_UNUSED(filepath);
    NM_CORE_ASSERT(false, "NOT IMPLEMENTED");

    return false;
}

}  // namespace nimbus