#pragma once

#include "nimbus/core/event.hpp"

#include <string>

namespace nimbus
{
class Layer : public refCounted
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

    // IMPORTANT: DO NOT perform any Graphics API or renderer calls in onUpdate.
    // It wont be deterministed because render thread, which handles those
    // updates only runs after onDraw!
    virtual void onUpdate(float deltaTime)
    {
        NM_UNUSED(deltaTime);
    }

    virtual void onDraw(float deltaTime)
    {
        NM_UNUSED(deltaTime);
    }

    virtual void onGuiDraw(float deltaTime)
    {
        NM_UNUSED(deltaTime);
    }

    // IMPORTANT: DO NOT perform any Graphics API or renderer calls in onEvent.
    // It wont be deterministed because render thread, which handles those
    // updates only runs after onDraw!
    virtual void onEvent(Event& event)
    {
        NM_UNUSED(event);
    }
};
}  // namespace nimbus