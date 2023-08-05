#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/guiLayers/guiSubsystem.hpp"

#include "nimbus/core/application.hpp"
#include "nimbus/renderer/graphicsApi.hpp"

///////////////////////
/// Dear Imgui stuff
//////////////////////
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"
#include "IconsFontAwesome6.h"

namespace nimbus
{

static const std::string k_defaultFontPath
    = "../resources/fonts/Roboto/Roboto-Regular.ttf";

static const std::string k_defaultIconFontPath
    = "../resources/fonts/FontAwesome6/" FONT_ICON_FILE_NAME_FAS;

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

    ///////////////////////////
    // Change font
    ///////////////////////////
    float   fontSize = 15.0f;
    ImFont* p_roboto
        = io.Fonts->AddFontFromFileTTF(k_defaultFontPath.c_str(), fontSize);

    if (p_roboto != nullptr)
    {
        io.FontDefault = p_roboto;
    }
    else
    {
        Log::coreCritical("Could not load GUI font: %s\n",
                          k_defaultFontPath.c_str());
    }

    ///////////////////////////
    // Add icons to font
    ///////////////////////////
    float baseFontSize = fontSize;

    // FontAwesome fonts need to have their sizes reduced
    // by 2.0f/3.0f in order to align correctly
    float iconFontSize = baseFontSize;

    // merge in icons from Font Awesome
    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
    ImFontConfig         icons_config;
    icons_config.MergeMode        = true;
    icons_config.PixelSnapH       = true;
    icons_config.GlyphMinAdvanceX = iconFontSize;
    ImFont* p_iconFont
        = io.Fonts->AddFontFromFileTTF(k_defaultIconFontPath.c_str(),
                                       iconFontSize,
                                       &icons_config,
                                       icons_ranges);

    if (p_iconFont == nullptr)
    {
        Log::coreCritical("Could not load GUI Icon font: %s\n",
                          k_defaultIconFontPath.c_str());
    }

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    // window is the SDL_Window*
    // context is the SDL_GLContext
    ImGui_ImplSDL2_InitForOpenGL(
        static_cast<SDL_Window*>(
            Application::s_get().getWindow().getOsWindow()),
        Application::s_get().getWindow().getContext());

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
            Application::s_get().setMenuMode(false);
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
    Application& app = Application::s_get();
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