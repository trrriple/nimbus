#pragma once
#include "nimbus.hpp"

#include "IconsFontAwesome6.h"

namespace nimbus
{

class ConsolePanel
{
   public:
    ConsolePanel()
    {
        mp_appRef    = &Application::s_get();
        mp_appWinRef = &mp_appRef->getWindow();
    }
    ~ConsolePanel()
    {
    }

    void onDraw()
    {
        ImGui::Begin("Console");

        Log::LogData& logData     = Log::s_getLogData();
        u32_t         tailIdx     = logData.tail;
        u32_t         quantity    = logData.quantity;
        u32_t         maxPossible = logData.msgs.size();

        bool gotNewMessages = false;
        if (tailIdx != m_lastTailIdx)
        {
            m_lastTailIdx  = tailIdx;
            gotNewMessages = true;
        }

        u32_t startIdx;
        if (quantity < maxPossible)
        {
            startIdx = 0;
        }
        else
        {
            startIdx = (tailIdx) % maxPossible;
        }

        // Text box for filtering
        static ImGuiTextFilter filter;
        filter.Draw(ICON_FA_FILTER);

        ImGui::SameLine();

        if (widgets::stateButton("Scroll Lock", m_scrollLock))
        {
            m_scrollLock = !m_scrollLock;
        }

        // do the filtering
        std::vector<i32_t> passedFilterEntries;
        passedFilterEntries.reserve(quantity);

        for (u32_t i = 0; i < quantity; i++)
        {
            u32_t idx = (startIdx + i) % maxPossible;

            if (filter.PassFilter(logData.msgs[idx].msg))
            {
                passedFilterEntries.push_back(idx);
            }
        }

        ImGui::BeginChild("#messages", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        ImGuiIO& io       = ImGui::GetIO();
        auto     monoFont = io.Fonts->Fonts[2];  // fixed width;
        ImGui::PushFont(monoFont);

        ImGuiListClipper clipper;
        clipper.Begin(passedFilterEntries.size());
        while (clipper.Step())
        {
            for (i32_t i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            {
                ImVec4 color;
                switch (logData.msgs[passedFilterEntries[i]].type)
                {
                    case (Log::Type::info):
                    {
                        color = ImVec4{0.46f, 0.87f, 0.14f, 1.0f};
                        break;
                    }
                    case (Log::Type::warn):
                    {
                        color = ImVec4{0.85f, 0.86f, 0.33f, 1.0f};
                        break;
                    }
                    case (Log::Type::error):
                    {
                        color = ImVec4{0.95f, 0.04f, 0.18f, 1.0f};
                        break;
                    }
                    case (Log::Type::critical):
                    {
                        color = ImVec4{0.42f, 0.0f, 0.0f, 1.0f};
                        break;
                    }
                    case (Log::Type::trace):
                    {
                        color = ImVec4{0.97f, 0.97f, 0.95f, 1.0f};
                        break;
                    }
                }

                ImGui::TextColored(color, "%s", logData.msgs[passedFilterEntries[i]].msg);
            }
        }
        clipper.End();

        ImGui::PopFont();

        if (m_scrollLock && gotNewMessages)
        {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();

        ImGui::End();
    }

   private:
    Application* mp_appRef;
    Window*      mp_appWinRef;
    bool         m_scrollLock  = true;
    u32_t        m_lastTailIdx = 0;
};

}  // namespace nimbus