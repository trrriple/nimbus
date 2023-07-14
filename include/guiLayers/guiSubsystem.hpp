#pragma once

#include "event.hpp"
#include "layer.hpp"

namespace nimbus
{
class GuiSubsystem : public Layer
{
   public:
    GuiSubsystem();

    virtual ~GuiSubsystem() = default;

    virtual void onInsert() override;

    virtual void onRemove() override;

    virtual void onEvent(Event& event) override;

    void begin();
    void end();

    void captureEvents(bool capture)
    {
        m_captureEvents = capture;
    }

   private:
    bool m_captureEvents = true;
};
}  // namespace nimbus