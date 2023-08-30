#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/scene/scene.hpp"
#include "nimbus/scene/component.hpp"
#include "nimbus/renderer/renderer2D.hpp"
#include "nimbus/renderer/renderer.hpp"
#include "nimbus/scene/entity.hpp"
#include "nimbus/scene/entityLogic.hpp"
#include "nimbus/script/scriptEngine.hpp"

namespace nimbus
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void _s_updateWorldTransform(TransformCmp& tc, AncestryCmp& ac)
{
    // if this guy has a parent, update his world transform
    if (ac.parent && ac.parent.hasComponent<TransformCmp>())
    {
        tc.world.setTransform(ac.parent.getComponent<TransformCmp>().world.getTransform() * tc.local.getTransform());
    }
    else
    {
        tc.world = tc.local;
    }

    // update his children's transforms, if any
    for (auto& child : ac.children)
    {
        if (child.hasComponent<TransformCmp>())
        {
            _s_updateWorldTransform(child.getComponent<TransformCmp>(), child.getComponent<AncestryCmp>());
        }
    }
}

// same as above, but accounts for physics behavior changes
static void _s_updateWorldTransformRuntime(TransformCmp& tc, AncestryCmp& ac, bool hasFixture)
{
    // if this guy has no fixture and a parent, update his world transform
    if (!hasFixture && ac.parent && ac.parent.hasComponent<TransformCmp>())
    {
        tc.world.setTransform(ac.parent.getComponent<TransformCmp>().world.getTransform() * tc.local.getTransform());
    }
    else
    {
        tc.world = tc.local;
    }

    // update his children's transforms, if any
    for (auto& child : ac.children)
    {
        if (child.hasComponent<TransformCmp>())
        {
            bool hasFixture = false;
            if (child.hasComponent<RigidBody2DCmp>())
            {
                if (child.getComponent<RigidBody2DCmp>().fixSpec.shape != nullptr)
                {
                    hasFixture = true;
                }
            }
            _s_updateWorldTransformRuntime(child.getComponent<TransformCmp>(),
                                           child.getComponent<AncestryCmp>(),
                                           hasFixture);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Scene::Scene(const std::string& name) : m_name(name)
{
}

Scene::~Scene()
{
}

Entity Scene::addEntity(const std::string& name)
{
    Entity entity = {m_registry.create(), this};

    // we pre-increment both to always start at one, and ensure when entities
    // are added using _addEntity, we account for that.
    entity.addComponent<GuidCmp>(++m_genesisIndex);

    entity.addComponent<NameCmp>(name.empty() ? entity.getComponent<GuidCmp>().guid.toString() : name);

    entity.addComponent<AncestryCmp>();

    return entity;
}

Entity Scene::addChildEntity(Entity parentEntity, const std::string& name)
{
    Entity child = addEntity(name);

    auto& childAc = child.getComponent<AncestryCmp>();

    childAc.parent = parentEntity;

    auto& parentAc = parentEntity.getComponent<AncestryCmp>();
    parentAc.children.push_back(child);

    return child;
}

void Scene::removeEntity(Entity entity, bool removeChildren)
{
    auto& ac = entity.getComponent<AncestryCmp>();

    if (!removeChildren)
    {
        // we want to remove this as the parent of all it's direct children
        // essentially promoting all the direct children to top level nodes
        for (auto child : ac.children)
        {
            child.getComponent<AncestryCmp>().parent = Entity();
        }
    }
    else
    {
        // otherwise we nuke the children recursively xD
        for (u32_t i = 0; i < ac.children.size(); i++)
        {
            removeEntity(ac.children[i], true);
        }
    }

    if (ac.parent)
    {
        auto& parentAc = ac.parent.getComponent<AncestryCmp>();

        auto it = std::find(parentAc.children.begin(), parentAc.children.end(), entity);

        if (it != parentAc.children.end())
        {
            parentAc.children.erase(it);
        }
    }

    m_registry.destroy(entity.getId());
}

void Scene::sortEntities()
{
    m_registry.sort<GuidCmp>([&](const auto lhs, const auto rhs) { return lhs.sequenceIndex < rhs.sequenceIndex; });
}

bool Scene::setScriptAssemblyPath(const std::filesystem::path& scriptAssemblyPath, bool load)
{
    std::filesystem::path relativePath;

    if (scriptAssemblyPath.root_name() != std::filesystem::current_path().root_name())
    {
        // Paths are on different drives
        relativePath = scriptAssemblyPath;
    }
    else
    {
        relativePath = std::filesystem::relative(scriptAssemblyPath);
    }

    m_scriptAssemblyPath = relativePath;

    if (load)
    {
        return loadScriptAssembly();
    }

    return true;
}

bool Scene::loadScriptAssembly()
{
    // unload any previously loaded assembly
    if(!unloadScriptAssembly())
    {
        return false;
    }

    if (!m_scriptAssemblyPath.empty())
    {
        if (ScriptEngine::s_loadScriptAssembly(m_scriptAssemblyPath))
        {
            m_scriptAssemblyTypeNames = ScriptEngine::s_getScriptAssemblyTypes();
            m_scriptAsssemblyLoaded   = true;
        }
        else
        {
            Log::coreCritical("Scene `%s` failed to load scene Script Assembly %s",
                              m_name.c_str(),
                              m_scriptAssemblyPath.generic_string().c_str());
            return false;
        }
    }
    else
    {
        Log::coreError("Scene `%s` can't load script assembly, path not set!", m_name.c_str());
        return false;
    }

    return true;
}

bool Scene::unloadScriptAssembly()
{
    if (m_scriptAsssemblyLoaded)
    {
        if (ScriptEngine::s_unloadScriptAssembly())
        {
            m_scriptAsssemblyLoaded = false;
            return true;
        }
        else
        {
            Log::coreError("Scene `%s` failed to unload script assembly", m_name.c_str());
            return false;
        }
    }

    return true;
}

void Scene::onStartRuntime()
{
    //////////////////////////////////////////////////////
    // Initialize Native Logic
    //////////////////////////////////////////////////////
    m_registry.view<NativeLogicCmp>().each(
        [=](auto entity, auto& nsc)
        {
            if (!nsc.p_logic)
            {
                nsc.p_logic           = nsc.initLogic();
                nsc.p_logic->m_entity = Entity{entity, this};
                nsc.p_logic->onCreate();
            }
        });

    //////////////////////////////////////////////////////
    // Initialize Scripts
    //////////////////////////////////////////////////////
    if (!m_scriptAsssemblyLoaded)
    {
        loadScriptAssembly();
    }

    if (m_scriptAsssemblyLoaded)
    {
        m_registry.view<ScriptCmp>().each(
            [=](auto entity, auto& sc)
            {
                if (!sc.scriptEntityName.empty())
                {
                    if (m_scriptAssemblyTypeNames.find(sc.scriptEntityName) != m_scriptAssemblyTypeNames.end())
                    {
                        sc.p_scriptInstance = ScriptEngine::s_createInstanceOfScriptAssemblyEntity(
                            sc.scriptEntityName, static_cast<u32_t>(entity));
                    }
                    else
                    {
                        Log::coreWarn("%s not found in %s", sc.scriptEntityName.c_str(), m_scriptAssemblyPath.c_str());
                    }
                }
            });
    }

    //////////////////////////////////////////////////////
    // Initialize Particle Emitters
    //////////////////////////////////////////////////////
    m_registry.view<ParticleEmitterCmp>().each(
        [=](auto entity, auto& pec)
        {
            NB_UNUSED(entity);
            pec.p_emitter = ref<ParticleEmitter>::gen(pec.numParticles, pec.parameters, pec.p_texture, nullptr, false);
        });

    //////////////////////////////////////////////////////
    // Make 2D Physics world
    //////////////////////////////////////////////////////
    mp_world2D = ref<Physics2D>::gen();

    m_registry.view<TransformCmp, RigidBody2DCmp, NameCmp>().each(
        [=](auto entity, auto& tc, auto& rbc, auto& nc)
        {
            NB_UNUSED(entity);

            // update the spec with transform information
            rbc.spec.position.x = tc.world.getTranslation().x;
            rbc.spec.position.y = tc.world.getTranslation().y;
            rbc.spec.angle      = tc.world.getRotation().z;

            // save off transform pre-sim to restore after
            rbc.preSimTransform = tc.local;

            // other parameters are configured in place and are accessable by the SHP
            rbc.p_body             = mp_world2D->addRigidBody(rbc.spec);
            rbc.p_body->name       = nc.name;
            rbc.p_body->p_userData = (void*)entity;

            // add fixture if so inclined
            if (rbc.fixSpec.shape != nullptr)
            {
                rbc.p_body->addFixture(rbc.fixSpec, tc.world);
            }
        });

    //////////////////////////////////////////////////////
    // Ensure correct aspect ratios on cameras
    //////////////////////////////////////////////////////
    m_registry.view<CameraCmp>().each(
        [=](auto entity, auto& cc)
        {
            NB_UNUSED(entity);
            if (!cc.fixedAspect)
            {
                cc.camera.setAspectRatio(m_aspectRatio);
            }
        });
}

void Scene::onStopRuntime()
{
    //////////////////////////////////////////////////////
    // Destruct Native Logic
    //////////////////////////////////////////////////////
    m_registry.view<NativeLogicCmp>().each(
        [=](auto entity, auto& nsc)
        {
            NB_UNUSED(entity);
            if (nsc.p_logic)
            {
                nsc.p_logic->onDestroy();
                delete nsc.p_logic;
                nsc.p_logic = nullptr;
            }
        });

    //////////////////////////////////////////////////////
    // Destroy Particle Emitters
    //////////////////////////////////////////////////////
    m_registry.view<ParticleEmitterCmp>().each(
        [=](auto entity, auto& pec)
        {
            NB_UNUSED(entity);
            pec.p_emitter = nullptr;
        });


    //////////////////////////////////////////////////////
    // Destroy Scripts
    //////////////////////////////////////////////////////
    m_registry.view<ScriptCmp>().each(
        [=](auto entity, auto& sc)
        {
            NB_UNUSED(entity);
            sc.p_scriptInstance = nullptr;
        });

    //////////////////////////////////////////////////////
    // Destory Physics World
    //////////////////////////////////////////////////////
    // remove all body refs
    m_registry.view<TransformCmp, RigidBody2DCmp>().each(
        [=](auto entity, auto& tc, auto& rbc)
        {
            NB_UNUSED(entity);
            rbc.p_body = nullptr;

            tc.local = rbc.preSimTransform;
        });

    // remove world
    mp_world2D = nullptr;
}

void Scene::onUpdateRuntime(f32_t deltaTime)
{
    //////////////////////////////////////////////////////
    // Update Physics
    //////////////////////////////////////////////////////
    mp_world2D->update(deltaTime);

    m_registry.view<TransformCmp, RigidBody2DCmp>().each(
        [=](auto entity, auto& tc, auto& rbc)
        {
            NB_UNUSED(entity);

            // we only want to update the XY translation and z rotation when using 2D physics
            util::Transform& transform = rbc.p_body->getTransform();
            tc.local.setTranslationX(transform.getTranslation().x);
            tc.local.setTranslationY(transform.getTranslation().y);
            tc.local.setRotationZ(transform.getRotation().z);
        });


    //////////////////////////////////////////////////////
    // Update Logic
    //////////////////////////////////////////////////////
    m_registry.view<NativeLogicCmp>().each(
        [=](auto entity, auto& nsc)
        {
            NB_UNUSED(entity);
            if (nsc.p_logic)
            {
                nsc.p_logic->onUpdate(deltaTime);
            }
        });


    //////////////////////////////////////////////////////
    // Update Scripts
    //////////////////////////////////////////////////////
    if (m_scriptAsssemblyLoaded)
    {
        m_registry.view<ScriptCmp>().each(
            [=](auto entity, auto& sc)
            {
                NB_UNUSED(entity);
                if (sc.p_scriptInstance)
                {
                    sc.p_scriptInstance->onUpdate(deltaTime);
                }
            });
    }

    // for all entities that have a transform, we want to update any
    // all child transforms accordingly
    auto tcView = m_registry.view<TransformCmp, AncestryCmp>();

    for (auto [entity, tc, ac] : tcView.each())
    {
        // only update top level here
        if (!ac.parent)
        {  // this parameter only matters for children
            _s_updateWorldTransformRuntime(tc, ac, false);
        }
    }

    auto peView = m_registry.view<GuidCmp, TransformCmp, ParticleEmitterCmp>();

    for (auto [entity, gc, tc, pec] : peView.each())
    {
        pec.p_emitter->updateSpawnTransform(tc.world.getTranslation(), tc.world.getRotation(), tc.world.getScale());
        pec.p_emitter->update(deltaTime);
    }

    for (auto&& fn : m_postUpdateWorkQueue)
    {
        fn();
    }
    m_postUpdateWorkQueue.clear();
}

void Scene::onDrawRuntime()
{
    //////////////////////////////////////////////////////
    // Get Camera
    //////////////////////////////////////////////////////
    auto cameraView = m_registry.view<CameraCmp>();

    Camera* p_mainCamera = nullptr;
    for (auto entity : cameraView)
    {
        auto& camera = cameraView.get<CameraCmp>(entity);

        // grab the first camera that's flagged to be used for rendering
        if (camera.primary == true)
        {
            p_mainCamera = &camera.camera;
            break;
        }
    }

    if (p_mainCamera == nullptr)
    {
        // need camera to render
        return;
    }

    _render(p_mainCamera);
    _renderSceneSpecific(p_mainCamera);
}

void Scene::onResize(u32_t width, u32_t height)
{
    auto cameraView = m_registry.view<CameraCmp>();

    m_aspectRatio = static_cast<f32_t>(width) / static_cast<f32_t>(height);

    for (auto entity : cameraView)
    {
        auto& camera = cameraView.get<CameraCmp>(entity);

        if (!camera.fixedAspect)
        {
            camera.camera.setAspectRatio(m_aspectRatio);
        }
    }
}

void Scene::_render(Camera* p_camera)
{
    ////////////////////////////////////////////////////////////////////////////
    // Render
    ////////////////////////////////////////////////////////////////////////////
    Renderer2D::s_begin(p_camera->getViewProjection());

    //////////////////////////////////////////////////////
    // Sprites
    //////////////////////////////////////////////////////
    auto spriteView = m_registry.view<GuidCmp, TransformCmp, SpriteCmp>();

    // order based on GuidCmp which should be sorted based on sequenceIndex
    // we want to render these by the order they were created, so newest
    // objects are on top (assuming = Z due to 2D)
    spriteView.use<GuidCmp>();

    for (auto [entity, gc, tc, sc] : spriteView.each())
    {
        Renderer2D::s_drawQuad(tc.world.getTransform(),
                               sc.p_texture,
                               sc.color,
                               sc.tilingFactor,
                               static_cast<int>(entity));
    }

    //////////////////////////////////////////////////////
    // Text
    //////////////////////////////////////////////////////
    auto textView = m_registry.view<GuidCmp, TransformCmp, TextCmp>();

    // same as for sprites, but we also assume we want to draw text after
    // sprites
    textView.use<GuidCmp>();

    for (auto [entity, gc, tc, txc] : textView.each())
    {
        Renderer2D::s_drawText(txc.text, txc.format, tc.world.getTransform(), static_cast<int>(entity));
    }

    Renderer2D::s_end();
}

void Scene::_renderSceneSpecific(Camera* p_camera)
{
    Renderer::s_setScene(p_camera->getViewProjection());
    //////////////////////////////////////////////////////
    // Particle Emitter
    //////////////////////////////////////////////////////
    auto peView = m_registry.view<GuidCmp, TransformCmp, ParticleEmitterCmp>();

    for (auto [entity, gc, tc, pec] : peView.each())
    {
        pec.p_emitter->draw();
    }
}

void Scene::_onUpdateEditor(f32_t deltaTime)
{
    NB_UNUSED(deltaTime);
    // for all entities that have a transform, we want to update any
    // all child transforms accordingly
    auto tcView = m_registry.view<TransformCmp, AncestryCmp>();

    for (auto [entity, tc, ac] : tcView.each())
    {
        // only update top level here
        if (!ac.parent)
        {
            _s_updateWorldTransform(tc, ac);
        }
    }
}

void Scene::_onDrawEditor(Camera* p_editorCamera)
{
    _render(p_editorCamera);
}

Entity Scene::_addEntity(const std::string& name, const std::string& guidStr, u32_t sequenceIndex)
{
    Entity entity = {m_registry.create(), this};

    entity.addComponent<GuidCmp>(sequenceIndex, guidStr);

    // TODO, is this right?
    if (sequenceIndex > m_genesisIndex)
    {
        m_genesisIndex = sequenceIndex;
    }

    entity.addComponent<NameCmp>(name);

    return entity;
}

}  // namespace nimbus