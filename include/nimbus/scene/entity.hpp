#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/scene/scene.hpp"

namespace nimbus
{

class Entity
{
   public:
    Entity() = default;
    Entity(entt::entity handle, Scene* scene)
        : mh_entity(handle), m_scene(scene)
    {
    }

    Entity(const Entity& other) = default;

    template <typename T>
    bool hasComponent()
    {
        return m_scene->m_registry.all_of<T>(mh_entity);
    }

    template <typename T>
    T& getComponent()
    {
        NM_ASSERT(hasComponent<T>(), "Entity does not have component!");

        return m_scene->m_registry.get<T>(mh_entity);
    }

    template <typename T, typename... Args>
    T& addComponent(Args&&... args)
    {
        NM_ASSERT(!hasComponent<T>(), "Entity already has component!");

        T& component = m_scene->m_registry.emplace<T>(
            mh_entity, std::forward<Args>(args)...);
        // m_scene->OnComponentAdded<T>(*this, component);
        return component;
    }

   private:
    entt::entity mh_entity{entt::null};
    Scene*       m_scene = nullptr;
};
}  // namespace nimbus