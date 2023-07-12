#pragma once

#include "core.hpp"
#include "event.hpp"

namespace nimbus
{
class Layer
{
   public:
    Layer();
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

    virtual void onEvent(Event& event)
    {
        UNUSED(event);
    }
};
}  // namespace nimbus