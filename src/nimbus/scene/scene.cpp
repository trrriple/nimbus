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

    return entity;
}

void Scene::removeEntity(Entity entity)
{
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

    auto peView = m_registry.view<GuidCmp, TransformCmp, ParticleEmitterCmp>();

    for (auto [entity, guid, transform, pec] : peView.each())
    {
        pec.p_emitter->update(deltaTime);
    }
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

    for (auto [entity, guid, transform, sprite] : spriteView.each())
    {
        Renderer2D::s_drawQuad(transform.getTransform(),
                               sprite.p_texture,
                               sprite.color,
                               sprite.tilingFactor,
                               static_cast<int>(entity));
    }

    ///////////////////////////
    // Text
    ///////////////////////////
    auto textView = m_registry.view<GuidCmp, TransformCmp, TextCmp>();

    // same as for sprites, but we also assume we want to draw text after
    // sprites
    textView.use<GuidCmp>();

    for (auto [entity, guid, transform, text] : textView.each())
    {
        Renderer2D::s_drawText(text.text,
                               text.format,
                               transform.getTransform(),
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
    auto peView
        = m_registry.view<GuidCmp, TransformCmp, ParticleEmitterCmp>();

    for (auto [entity, guid, transform, pec] : peView.each())
    {
        pec.p_emitter->draw(transform.getTransform());
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