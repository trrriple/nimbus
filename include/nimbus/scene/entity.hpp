#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/scene/scene.hpp"
#include "entt/entity/registry.hpp"

namespace nimbus
{

class Entity
{
   public:
    inline static const uint32_t k_nullEntity = entt::null;

    Entity() = default;
    Entity(entt::entity handle, Scene* scene) noexcept
        : mh_entity(handle), mp_sceneParent(scene)
    {
    }

    template <typename T>
    inline bool hasComponent() noexcept
    {
        return mp_sceneParent->m_registry.all_of<T>(mh_entity);
    }

    template <typename T>
    inline T& getComponent() noexcept
    {
        NM_ASSERT(hasComponent<T>(), "Entity does not have component!");

        return mp_sceneParent->m_registry.get<T>(mh_entity);
    }

    template <typename T, typename... Args>
    inline T& addComponent(Args&&... args) noexcept
    {
        NM_ASSERT(!hasComponent<T>(), "Entity already has component!");

        T& component = mp_sceneParent->m_registry.emplace<T>(
            mh_entity, std::forward<Args>(args)...);
        // mp_sceneParent->OnComponentAdded<T>(*this, component);
        return component;
    }

    template <typename T, typename... Args>
    inline void removeComponent() noexcept
    {
        NM_ASSERT(hasComponent<T>(), "Entity does not have component!");
        mp_sceneParent->m_registry.remove<T>(mh_entity);
    }

    inline entt::entity getId() noexcept
    {
        return mh_entity;
    }

    inline operator bool() const noexcept
    {
        return mh_entity != entt::null;
    }

    inline bool operator==(const Entity& other) const noexcept
    {
        return (other.mh_entity == mh_entity
                && other.mp_sceneParent == mp_sceneParent);
    }

    inline bool operator!=(const Entity& other) const noexcept
    {
        return !(*this == other);
    }

   private:
    entt::entity mh_entity{entt::null};
    Scene*       mp_sceneParent = nullptr;
};
}  // namespace nimbus