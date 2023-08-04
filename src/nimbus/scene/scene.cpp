#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/scene/scene.hpp"
#include "nimbus/scene/component.hpp"
#include "nimbus/renderer/renderer2D.hpp"
#include "nimbus/scene/entity.hpp"
#include "nimbus/scene/entityLogic.hpp"

namespace nimbus
{

Scene::Scene()
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
    entity.addComponent<NameCmp>(name.empty() ? "UUID" : name);

    return entity;
}

void Scene::onStart()
{
    ///////////////////////////
    // Initialize Logic
    ///////////////////////////
    m_registry.view<nativeLogicCmp>().each(
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
    m_registry.view<nativeLogicCmp>().each(
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
    m_registry.view<nativeLogicCmp>().each(
        [=](auto entity, auto& nsc)
        {
            NM_UNUSED(entity);
            if (nsc.p_logic)
            {
                nsc.p_logic->onUpdate(deltaTime);
            }
        });
}

void Scene::onDraw(float deltaTime)
{
    NM_UNUSED(deltaTime);
    ///////////////////////////
    // Get Camera
    ///////////////////////////
    auto cameraView = m_registry.view<CameraCmp>();

    Camera* mainCamera = nullptr;
    for (auto entity : cameraView)
    {
        auto& camera = cameraView.get<CameraCmp>(entity);

        // grab the first camera that's flagged to be used for rendering
        if (camera.renderWith == true)
        {
            mainCamera = camera.p_camera.get();
            break;
        }
    }

    if (mainCamera == nullptr)
    {
        // need camera to render
        return;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Render
    ////////////////////////////////////////////////////////////////////////////
    Renderer2D::s_begin(mainCamera->getViewProjection());

    ///////////////////////////
    // Sprites
    ///////////////////////////
    auto spriteGroup = m_registry.group<TransformCmp>(entt::get<SpriteCmp>);
    for (auto entity : spriteGroup)
    {
        auto [transform, sprite]
            = spriteGroup.get<TransformCmp, SpriteCmp>(entity);

        Renderer2D::s_drawQuad(transform.getTransform(),
                               sprite.texture,
                               sprite.color,
                               sprite.tilingFactor);
    }

    ///////////////////////////
    // Text
    ///////////////////////////
    auto textView = m_registry.view<TransformCmp, TextCmp>();
    for (auto entity : textView)
    {
        auto [transform, text] = textView.get<TransformCmp, TextCmp>(entity);

        Renderer2D::s_drawText(
            text.text, text.format, transform.getTransform());
    }

    Renderer2D::s_end();
}

void Scene::onResize(uint32_t width, uint32_t height)
{
    auto cameraView = m_registry.view<CameraCmp>();

    const float aspectRatio
        = static_cast<float>(width) / static_cast<float>(height);

    for (auto entity : cameraView)
    {
        auto& camera = cameraView.get<CameraCmp>(entity);

        if (!camera.fixedAspect)
        {
            camera.p_camera->setAspectRatio(aspectRatio);
        }
    }
}

}  // namespace nimbus