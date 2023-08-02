#pragma once

#include "nimbus/core/event.hpp"
#include "nimbus/core/layer.hpp"

namespace nimbus
{
class EngineGui : public Layer
{
    inline static const uint32_t k_frameHistoryLength = 60 * 2 + 1;

   public:
    EngineGui();

    virtual ~EngineGui() = default;

    virtual void onInsert() override;

    virtual void onRemove() override;

    virtual void onEvent(Event& event) override;

    virtual void onGuiUpdate(float deltaTime) override;

   private:
    std::vector<float> m_frameTimes_ms;
};
}  // namespace nimbus