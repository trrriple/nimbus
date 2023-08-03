#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/scene/scene.hpp"
#include "nimbus/scene/component.hpp"
#include "nimbus/renderer/renderer2D.hpp"
#include "nimbus/scene/entity.hpp"

namespace nimbus
{

Scene::Scene()
{
}

Scene::~Scene()
{
}

Entity Scene::addEntity()
{
    Entity entity = {m_registry.create(), this};

    return entity;
}

void Scene::onUpdate(float deltaTime)
{
    ///////////////////////////
    // Get Camera
    ///////////////////////////
    auto cameraView = m_registry.view<CameraCmp>();

    Camera* mainCamera = nullptr;
    for (auto entity : cameraView)
    {
        auto& camera = cameraView.get<CameraCmp>(entity);

        mainCamera = &camera.camera;

    }

    if(mainCamera == nullptr)
    {
        // need camera to render
        return; 
    }

    ///////////////////////////
    // Begin rendering
    ///////////////////////////
    Renderer2D::s_begin(*mainCamera);

    // sprites
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


    Renderer2D::s_end();

}

}  // namespace nimbus