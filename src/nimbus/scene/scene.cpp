#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/scene/scene.hpp"
#include "nimbus/scene/component.hpp"
#include "nimbus/renderer/renderer2D.hpp"
#include "nimbus/renderer/renderer.hpp"
#include "nimbus/scene/entity.hpp"
#include "nimbus/scene/entityLogic.hpp"

namespace nimbus
{

////////////////////////////////////////////////////////////////////////////////
// Static functions
////////////////////////////////////////////////////////////////////////////////
static void _s_updateWorldTransform(TransformCmp& tc, AncestryCmp& ac)
{
    // if this guy has a parent, update his world transform
    if (ac.parent && ac.parent.hasComponent<TransformCmp>())
    {
        tc.world.setTransform(
            ac.parent.getComponent<TransformCmp>().world.getTransform()
            * tc.local.getTransform());
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
            _s_updateWorldTransform(child.getComponent<TransformCmp>(),
                                    child.getComponent<AncestryCmp>());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////
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

    entity.addComponent<NameCmp>(
        name.empty() ? entity.getComponent<GuidCmp>().guid.toString() : name);

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
        for (uint32_t i = 0; i < ac.children.size(); i++)
        {
            removeEntity(ac.children[i], true);
        }
    }

    if (ac.parent)
    {
        auto& parentAc = ac.parent.getComponent<AncestryCmp>();

        auto it = std::find(
            parentAc.children.begin(), parentAc.children.end(), entity);

        if (it != parentAc.children.end())
        {
            parentAc.children.erase(it);
        }
    }

    m_registry.destroy(entity.getId());
}

void Scene::sortEntities()
{
    m_registry.sort<GuidCmp>([&](const auto lhs, const auto rhs)
                             { return lhs.sequenceIndex < rhs.sequenceIndex; });
}

void Scene::onStart()
{
    ///////////////////////////
    // Initialize Logic
    ///////////////////////////
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

    m_registry.view<ParticleEmitterCmp>().each(
        [=](auto entity, auto& pec)
        {
            NM_UNUSED(entity);
            pec.p_emitter = ref<ParticleEmitter>::gen(pec.numParticles,
                                                      pec.parameters,
                                                      pec.p_texture,
                                                      nullptr,
                                                      false);
        });
}

void Scene::onStop()
{
    ///////////////////////////
    // Destruct Logic
    ///////////////////////////
    m_registry.view<NativeLogicCmp>().each(
        [=](auto entity, auto& nsc)
        {
            NM_UNUSED(entity);
            if (nsc.p_logic)
            {
                nsc.p_logic->onDestroy();
                delete nsc.p_logic;
                nsc.p_logic = nullptr;
            }
        });

    m_registry.view<ParticleEmitterCmp>().each(
        [=](auto entity, auto& pec)
        {
            NM_UNUSED(entity);
            pec.p_emitter = nullptr;
        });
}

void Scene::onUpdate(float deltaTime)
{
    NM_UNUSED(deltaTime);

    ///////////////////////////
    // Update Logic
    ///////////////////////////
    m_registry.view<NativeLogicCmp>().each(
        [=](auto entity, auto& nsc)
        {
            NM_UNUSED(entity);
            if (nsc.p_logic)
            {
                nsc.p_logic->onUpdate(deltaTime);
            }
        });


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

    auto peView = m_registry.view<GuidCmp, TransformCmp, ParticleEmitterCmp>();

    for (auto [entity, gc, tc, pec] : peView.each())
    {
        pec.p_emitter->updateSpawnTransform(tc.world.getTranslation(),
                                            tc.world.getRotation(),
                                            tc.world.getScale());
        pec.p_emitter->update(deltaTime);
    }

    for (auto&& fn : m_postUpdateWorkQueue)
    {
        fn();
    }
    m_postUpdateWorkQueue.clear();
}

void Scene::onDraw()
{
    ///////////////////////////
    // Get Camera
    ///////////////////////////
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

void Scene::onResize(uint32_t width, uint32_t height)
{
    auto cameraView = m_registry.view<CameraCmp>();

    m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);

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

    ///////////////////////////
    // Sprites
    ///////////////////////////
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

    ///////////////////////////
    // Text
    ///////////////////////////
    auto textView = m_registry.view<GuidCmp, TransformCmp, TextCmp>();

    // same as for sprites, but we also assume we want to draw text after
    // sprites
    textView.use<GuidCmp>();

    for (auto [entity, gc, tc, txc] : textView.each())
    {
        Renderer2D::s_drawText(txc.text,
                               txc.format,
                               tc.world.getTransform(),
                               static_cast<int>(entity));
    }


    Renderer2D::s_end();
}

void Scene::_renderSceneSpecific(Camera* p_camera)
{
    Renderer::s_setScene(p_camera->getViewProjection());
    ///////////////////////////
    // Particle Emitter
    ///////////////////////////
    auto peView = m_registry.view<GuidCmp, TransformCmp, ParticleEmitterCmp>();

    for (auto [entity, gc, tc, pec] : peView.each())
    {
        pec.p_emitter->draw();
    }
}

void Scene::_onUpdateEditor(float deltaTime)
{
    NM_UNUSED(deltaTime);
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

Entity Scene::_addEntity(const std::string& name,
                         const std::string& guidStr,
                         uint32_t           sequenceIndex)
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