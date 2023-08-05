#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/core/application.hpp"
#include "nimbus/core/window.hpp"
#include "IconsFontAwesome6.h"


namespace nimbus
{

class SceneControlPanel
{
   public:

    enum class State
    {
        PAUSE,
        PLAY
    };

    bool m_wireFrame = false;

    SceneControlPanel()
    {
        mp_appRef    = &Application::s_get();
        mp_appWinRef = &mp_appRef->getWindow();

        m_state = State::PAUSE;


    }
    ~SceneControlPanel()
    {
    }

    void onDraw()
    {

        ImGui::Begin("Scene Control");

        bool play = m_state == State::PLAY;

        char icon[5];

        if(play)
        {
            snprintf(icon, 5, ICON_FA_PAUSE);
        }
        else
        {
            snprintf(icon, 5, ICON_FA_PLAY);
        }

        if(ImGui::Button(icon))
        {
            m_state = play ?  State::PAUSE : State::PLAY;
        }

        ImGui::End();
    }

    State getState()
    {
        return m_state;
    }

   private:
    Application* mp_appRef;
    Window*      mp_appWinRef;

    State        m_state;

};

}  // namespace nimbus