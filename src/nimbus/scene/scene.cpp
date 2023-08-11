#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/scene/scene.hpp"
#include "nimbus/scene/component.hpp"
#include "nimbus/renderer/renderer2D.hpp"
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

    // TODO: default name to the UUID of the entity
    // add a name component because we'll probably want one

    entity.addComponent<GuidCmp>();

    entity.addComponent<NameCmp>(
        name.empty() ? entity.getComponent<GuidCmp>().guid.toString() : name);

    return entity;
}

void Scene::removeEntity(Entity entity)
{
    m_registry.destroy(entity.getId());
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
    auto spriteGroup = m_registry.group<TransformCmp>(entt::get<SpriteCmp>);
    for (auto entity : spriteGroup)
    {
        auto [transform, sprite]
            = spriteGroup.get<TransformCmp, SpriteCmp>(entity);

        Renderer2D::s_drawQuad(transform.getTransform(),
                               sprite.p_texture,
                               sprite.color,
                               sprite.tilingFactor,
                               static_cast<int>(entity));
    }

    ///////////////////////////
    // Text
    ///////////////////////////
    auto textView = m_registry.view<TransformCmp, TextCmp>();
    for (auto entity : textView)
    {
        auto [transform, text] = textView.get<TransformCmp, TextCmp>(entity);

        Renderer2D::s_drawText(text.text,
                               text.format,
                               transform.getTransform(),
                               static_cast<int>(entity));
    }

    Renderer2D::s_end();
}

void Scene::_onDrawEditor(Camera* p_editorCamera)
{
    _render(p_editorCamera);
}

}  // namespace nimbus