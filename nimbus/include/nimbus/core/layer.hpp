#pragma once

#include "nimbus/core/event.hpp"

#include <string>

namespace nimbus
{
class NIMBUS_API Layer : public refCounted
{
   public:
    enum class Type
    {
        regular,
        overlay
    };

    Type        m_type;
    std::string m_name;

    Layer(Type type = Type::regular, const std::string& name = "noName");

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
    virtual void onUpdate(f32_t deltaTime)
    {
        NB_UNUSED(deltaTime);
    }

    virtual void onDraw(f32_t deltaTime)
    {
        NB_UNUSED(deltaTime);
    }

    virtual void onGuiDraw(f32_t deltaTime)
    {
        NB_UNUSED(deltaTime);
    }

    // IMPORTANT: DO NOT perform any Graphics API or renderer calls in onEvent.
    // It wont be deterministed because render thread, which handles those
    // updates only runs after onDraw!
    virtual void onEvent(Event& event)
    {
        NB_UNUSED(event);
    }
};
}  // namespace nimbus