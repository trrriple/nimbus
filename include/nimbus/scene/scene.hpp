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

    Entity addEntity(const std::string& name = std::string());
    
    void onStart();
    void onUpdate(float deltaTime);
    void onDraw(float deltaTime);
    void onStop();

    void onResize(uint32_t width, uint32_t height);

   private:
    entt::registry m_registry;

    friend class Entity;

};

}  // namespace nimbus