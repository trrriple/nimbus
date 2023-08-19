#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/core/application.hpp"
#include "nimbus/core/window.hpp"
#include "nimbus/guiSubsystem/widgets.hpp"

#include "IconsFontAwesome6.h"

namespace nimbus
{

class SceneControlPanel
{
   public:
    enum class RunState
    {
        STOP,
        PLAY
    };

    enum class ToolState
    {
        NONE,
        UNIVERSAL,
        MOVE,
        ROTATE,
        SCALE,
    };

    struct State
    {
        RunState  runState  = RunState::STOP;
        ToolState toolState = ToolState::NONE;
    };

    bool m_wireFrame = false;

    SceneControlPanel()
    {
        mp_appRef    = &Application::s_get();
        mp_appWinRef = &mp_appRef->getWindow();

        m_state.runState = RunState::STOP;
    }
    ~SceneControlPanel()
    {
    }

    void onDraw()
    {
        // overlay
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking
                                       | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing
                                       | ImGuiWindowFlags_NoNav;

        ImGui::Begin("Scene Control", nullptr, windowFlags);

        ImGui::BeginTable("##Controls", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        bool play = m_state.runState == RunState::PLAY;

        char icon[5];

        ///////////////////////////
        // Run State
        ///////////////////////////
        if (play)
        {
            snprintf(icon, 5, ICON_FA_STOP);
        }
        else
        {
            snprintf(icon, 5, ICON_FA_PLAY);
        }

        if (ImGui::Button(icon))
        {
            m_state.runState = play ? RunState::STOP : RunState::PLAY;
        }

        ///////////////////////////
        // Manipulation Tools
        ///////////////////////////
        ImGui::TableNextColumn();
        ImGui::SameLine();

        // universal tool
        bool enabled = m_state.toolState == ToolState::UNIVERSAL;
        if (widgets::stateButton(ICON_FA_ATOM, enabled))
        {
            if (enabled)
            {
                // turn off tool
                m_state.toolState = ToolState::NONE;
            }
            else
            {
                // enable/switch tool
                m_state.toolState = ToolState::UNIVERSAL;
            }
        }

        ImGui::SameLine();

        // move tool
        enabled = m_state.toolState == ToolState::MOVE;
        if (widgets::stateButton(ICON_FA_UP_DOWN_LEFT_RIGHT, enabled))
        {
            if (enabled)
            {
                // turn off tool
                m_state.toolState = ToolState::NONE;
            }
            else
            {
                // enable/switch tool
                m_state.toolState = ToolState::MOVE;
            }
        }

        ImGui::SameLine();

        // rotate tool
        enabled = m_state.toolState == ToolState::ROTATE;
        if (widgets::stateButton(ICON_FA_ARROWS_SPIN, enabled))
        {
            if (enabled)
            {
                // turn off tool
                m_state.toolState = ToolState::NONE;
            }
            else
            {
                // enable/switch tool
                m_state.toolState = ToolState::ROTATE;
            }
        }

        ImGui::SameLine();
        // scale tool
        enabled = m_state.toolState == ToolState::SCALE;
        if (widgets::stateButton(ICON_FA_MAXIMIZE, enabled))
        {
            if (enabled)
            {
                // turn off tool
                m_state.toolState = ToolState::NONE;
            }
            else
            {
                // enable/switch tool
                m_state.toolState = ToolState::SCALE;
            }
        }

        ImGui::EndTable();

        ImGui::End();
    }

    State& getState()
    {
        return m_state;
    }

    void setRunState(RunState runState)
    {
        m_state.runState = runState;
    }

   private:
    Application* mp_appRef;
    Window*      mp_appWinRef;

    State m_state;
};

}  // namespace nimbus