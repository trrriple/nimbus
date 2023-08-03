#pragma once

#include "nimbus/core/event.hpp"

#include <string>

namespace nimbus
{
class Layer
{
   public:
    enum class Type
    {
        REGULAR,
        OVERLAY
    };

    Type        m_type;
    std::string m_name;

    Layer(Type type = Type::REGULAR, const std::string& name = "noName");

    virtual ~Layer() = default;

    virtual void onInsert()
    {
    }

    virtual void onRemove()
    {
    }

    virtual void onUpdate(float deltaTime)
    {
        NM_UNUSED(deltaTime);
    }
    
    virtual void onDraw(float deltaTime)
    {
        NM_UNUSED(deltaTime);
    }

    virtual void onEvent(Event& event)
    {
        NM_UNUSED(event);
    }

    virtual void onGuiUpdate(float deltaTime)
    {
        NM_UNUSED(deltaTime);
    }


    
};
}  // namespace nimbus