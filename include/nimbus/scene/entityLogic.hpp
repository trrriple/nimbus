#pragma once

#include "nimbus/scene/scene.hpp"
#include "nimbus/scene/entity.hpp"

namespace nimbus
{

class EntityLogic
{
   public:
    virtual ~EntityLogic()
    {
    }

    template <typename T>
    T& getComponent()
    {
        return m_entity.getComponent<T>();
    }

   protected:
    virtual void onCreate()
    {
    }
    virtual void onDestroy()
    {
    }
    virtual void onUpdate(f32_t deltaTime)
    {
        NB_UNUSED(deltaTime);
    }

   private:
    Entity m_entity;
    friend class Scene;
};

}  // namespace nimbus
