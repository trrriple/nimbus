#include "nmpch.hpp"
#include "core.hpp"

#include "guiLayers/guiSubsystem.hpp"
#include "application.hpp"
#include "platform/rendererApi.hpp"

///////////////////////
/// Dear Imgui stuff
//////////////////////
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"

namespace nimbus
{

GuiSubsystem::GuiSubsystem() : Layer(Layer::Type::OVERLAY, "guiSubsystem")
{
}

void GuiSubsystem::onInsert()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    // window is the SDL_Window*
    // context is the SDL_GLContext
    ImGui_ImplSDL2_InitForOpenGL(
        static_cast<SDL_Window*>(Application::get().getWindow().getOsWindow()),
        Application::get().getWindow().getContext());
    
    ImGui_ImplOpenGL3_Init();
}

void GuiSubsystem::onRemove()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void GuiSubsystem::onEvent(Event& event)
{
    ImGui_ImplSDL2_ProcessEvent((SDL_Event*)&event.getDetails());

    if (m_captureEvents)
    {
        Event::Type eventType = event.getEventType();

        ImGuiIO& io = ImGui::GetIO();

        // mark mouse events as handled
        if (io.WantCaptureMouse
            && (eventType == Event::Type::MOUSEMOTION
                || eventType == Event::Type::MOUSEBUTTONUP
                || eventType == Event::Type::MOUSEBUTTONDOWN
                || eventType == Event::Type::MOUSEWHEEL))
        {
            event.markAsHandled();
        }
        // mark keyboard events as handled
        else if (io.WantCaptureKeyboard
                 && (eventType == Event::Type::KEYDOWN
                     || eventType == Event::Type::KEYUP
                     || eventType == Event::Type::TEXTEDITING
                     || eventType == Event::Type::TEXTINPUT))
        {
            event.markAsHandled();
        }
        else if (!io.WantCaptureMouse
                 && eventType == Event::Type::MOUSEBUTTONDOWN)
        {
            Application::get().setMenuMode(false);
        }
    }
}

void GuiSubsystem::begin()
{
    NM_PROFILE_DETAIL();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void GuiSubsystem::end()
{
    NM_PROFILE_DETAIL();

    ImGuiIO&     io  = ImGui::GetIO();
    Application& app = Application::get();
    io.DisplaySize   = ImVec2((float)app.getWindow().getWidth(),
                            (float)app.getWindow().getHeight());

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        SDL_Window*   backupCurrentWindow  = SDL_GL_GetCurrentWindow();
        SDL_GLContext backupCurrentContext = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(backupCurrentWindow, backupCurrentContext);
    }

}

}  // namespace nimbus