#pragma once
#include "nimbus.hpp"

#include "IconsFontAwesome6.h"

namespace nimbus
{

class ResourcePanel
{
   public:
    ResourcePanel()
    {
        mp_appRef    = &Application::s_get();
        mp_appWinRef = &mp_appRef->getWindow();
    }
    ~ResourcePanel()
    {
    }

    void onDraw()
    {
        ImGui::Begin("Resources");

        ImGui::End();
    }

   private:
    Application* mp_appRef;
    Window*      mp_appWinRef;

};
}  // namespace nimbus