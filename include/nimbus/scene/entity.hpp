#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/scene/scene.hpp"
#include "entt/entity/registry.hpp"

namespace nimbus
{

class Entity
{
   public:
    Entity() = default;
    Entity(entt::entity handle, Scene* scene)
        : mh_entity(handle), mp_sceneParent(scene)
    {
    }

    template <typename T>
    bool hasComponent()
    {
        return mp_sceneParent->m_registry.all_of<T>(mh_entity);
    }

    template <typename T>
    T& getComponent()
    {
        NM_ASSERT(hasComponent<T>(), "Entity does not have component!");

        return mp_sceneParent->m_registry.get<T>(mh_entity);
    }

    template <typename T, typename... Args>
    T& addComponent(Args&&... args)
    {
        NM_ASSERT(!hasComponent<T>(), "Entity already has component!");

        T& component = mp_sceneParent->m_registry.emplace<T>(
            mh_entity, std::forward<Args>(args)...);
        // mp_sceneParent->OnComponentAdded<T>(*this, component);
        return component;
    }

    template <typename T, typename... Args>
    void removeComponent()
    {
        NM_ASSERT(hasComponent<T>(), "Entity does not have component!");
        mp_sceneParent->m_registry.remove<T>(mh_entity);
    }

    entt::entity getId()
    {
        return mh_entity;
    }

    operator bool() const
    {
        return mh_entity != entt::null;
    }

    bool operator==(const Entity& other) const
    {
        return (other.mh_entity == mh_entity
                && other.mp_sceneParent == mp_sceneParent);
    }

    bool operator!=(const Entity& other) const
    {
        return !(*this == other);
    }

   private:
    entt::entity mh_entity{entt::null};
    Scene*       mp_sceneParent = nullptr;
};
}  // namespace nimbus