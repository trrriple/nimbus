#pragma once

#include "event.hpp"

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

    virtual void onUpdate()
    {
    }
    
    virtual void onGuiUpdate()
    {
    }

    virtual void onEvent(Event& event)
    {
        UNUSED(event);
    }
    
};
}  // namespace nimbus