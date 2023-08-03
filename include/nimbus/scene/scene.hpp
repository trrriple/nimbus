#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/scene/component.hpp"


#include "entt/entity/registry.hpp"

namespace nimbus
{

class Entity; // forward declare, can't include header for circular reason


class Scene
{
   public:
    Scene();
    ~Scene();

    Entity addEntity();
    

    void onUpdate(float deltaTime);

   private:
    entt::registry m_registry;

    friend class Entity;

};

}  // namespace nimbus