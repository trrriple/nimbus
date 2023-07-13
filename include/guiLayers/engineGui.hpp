#pragma once

#include "core.hpp"
#include "event.hpp"
#include "layer.hpp"

namespace nimbus
{
class EngineGui : public Layer
{
   public:
    EngineGui();

    virtual ~EngineGui() = default;

    virtual void onInsert() override;

    virtual void onRemove() override;

    virtual void onEvent(Event& event) override;

    virtual void onGuiUpdate() override;

};
}  // namespace nimbus