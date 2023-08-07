#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/guiLayers/guiSubsystem.hpp"

#include "nimbus/core/application.hpp"
#include "nimbus/renderer/graphicsApi.hpp"
#include "nimbus/renderer/renderer.hpp"

///////////////////////
/// Dear Imgui stuff
//////////////////////
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "IconsFontAwesome6.h"


namespace nimbus
{

static const std::string k_defaultFontPath
    = "../resources/fonts/Roboto/Roboto-Regular.ttf";

static const std::string k_defaultBoldFontPath
    = "../resources/fonts/Roboto/Roboto-Black.ttf";

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

    ///////////////////////////
    // Bold font
    ///////////////////////////
    ImFont* p_robotoBold
        = io.Fonts->AddFontFromFileTTF(k_defaultBoldFontPath.c_str(), fontSize);

    if (p_robotoBold == nullptr)
    {
        Log::coreCritical("Could not load GUI font: %s\n",
                          k_defaultFontPath.c_str());
    }

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    //override some values;
    _setDarkThemeColors();

    // Setup Platform/Renderer bindings
    // window is the SDL_Window*
    // context is the SDL_GLContext

    auto p_window = static_cast<SDL_Window*>(
        Application::s_get().getWindow().getOsWindow());

    auto context = Application::s_get().getWindow().getContext();

    ImGui_ImplSDL2_InitForOpenGL(p_window, context);

    bool imguiInit = ImGui_ImplOpenGL3_Init();

    NM_CORE_ASSERT(imguiInit, "Failed to initialize Imgui!");
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
    ImGuizmo::BeginFrame();
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

void GuiSubsystem::_setDarkThemeColors()
{
    auto& colors              = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{0.12f, 0.12f, 0.11f, 1.0f};

    // Headers
    colors[ImGuiCol_Header]        = ImVec4{0.2f, 0.21f, 0.19f, 1.0f};
    colors[ImGuiCol_HeaderHovered] = ImVec4{0.24f, 0.24f, 0.2f, 1.0f};
    colors[ImGuiCol_HeaderActive]  = ImVec4{0.46f, 0.44f, 0.37f, 1.0f};

    // Buttons
    colors[ImGuiCol_Button]        = ImVec4{0.2f, 0.21f, 0.19f, 1.0f};
    colors[ImGuiCol_ButtonHovered] = ImVec4{0.24f, 0.24f, 0.2f, 1.0f};
    colors[ImGuiCol_ButtonActive]  = ImVec4{0.46f, 0.44f, 0.37f, 1.0f};

    // Frame BG
    colors[ImGuiCol_FrameBg]        = ImVec4{0.2f, 0.21f, 0.19f, 1.0f};
    colors[ImGuiCol_FrameBgHovered] = ImVec4{0.24f, 0.24f, 0.2f, 1.0f};
    colors[ImGuiCol_FrameBgActive]  = ImVec4{0.46f, 0.44f, 0.37f, 1.0f};

    // Tabs
    colors[ImGuiCol_Tab]                = ImVec4{0.46f, 0.44f, 0.37f, 1.0f};
    colors[ImGuiCol_TabHovered]         = ImVec4{0.25f, 0.26f, 0.22f, 1.0f};
    colors[ImGuiCol_TabActive]          = ImVec4{0.25f, 0.26f, 0.22f, 1.0f};
    colors[ImGuiCol_TabUnfocused]       = ImVec4{0.46f, 0.44f, 0.37f, 1.0f};
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.2f, 0.21f, 0.19f, 1.0f};

    // Title
    colors[ImGuiCol_TitleBg]          = ImVec4{0.46f, 0.44f, 0.37f, 1.0f};
    colors[ImGuiCol_TitleBgActive]    = ImVec4{0.46f, 0.44f, 0.37f, 1.0f};
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.46f, 0.44f, 0.37f, 1.0f};

    // Table
    colors[ImGuiCol_TableHeaderBg]     = ImVec4{0.2f, 0.21f, 0.19f, 1.0f};

    // misc
    colors[ImGuiCol_CheckMark]        = ImVec4{0.67f, 0.84f, 0.0f, 1.0f};
    colors[ImGuiCol_SeparatorHovered] = ImVec4{0.454f, 0.567f, 0.0f, 1.0f};
    colors[ImGuiCol_SeparatorActive]  = ImVec4{0.67f, 0.84f, 0.0f, 1.0f};
}

}  // namespace nimbus