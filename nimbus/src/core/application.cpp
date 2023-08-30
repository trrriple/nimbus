#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/core/application.hpp"
#include "nimbus/core/event.hpp"

#include "nimbus/script/scriptEngine.hpp"

#include "nimbus/guiSubsystem/guiSubsystem.hpp"
#include "nimbus/renderer/renderer.hpp"
#include "nimbus/renderer/renderer2D.hpp"

#include "nimbus/renderer/font.hpp"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"

namespace nimbus
{

Application* Application::sp_instance = nullptr;

Application::Application(const std::string& name, u32_t windowWidth, u32_t windowHeight) : m_name(name)
{
    NB_CORE_ASSERT(!sp_instance, "Application should only be created once!\n");

    Log::s_init();

    sp_instance = this;

    mp_window = genScope<Window>(m_name, windowWidth, windowHeight);

    Log::coreInfo("------------------------------------------");
    Log::coreInfo("----- Nimbus Engine Application Init -----");
    Log::coreInfo("------------------------------------------");

    mp_resourceManager = genScope<ResourceManager>();

    mp_window->graphicsContextInit();

    mp_window->setEventCallback(std::bind(&Application::onEvent, this, std::placeholders::_1));

    mp_window->setExitCallback(std::bind(&Application::shouldQuit, this, std::placeholders::_1));

    mp_guiSubsystemLayer = ref<GuiSubsystem>::gen();

    insertLayer(mp_guiSubsystemLayer);

    Renderer::s_init();

    Renderer2D::s_init();

    // TODO configurable parameter
    ScriptEngine::s_init("somePath?");
}

Application::~Application()
{
    ScriptEngine::s_destroy();

    // pump all the commands out before we clear layers as we odn't want
    // pending draw commands to use deleted resources
    Renderer::s_pumpCmds();

    m_layerDeck.clear();
    mp_resourceManager.reset();

    Renderer2D::s_destroy();

    // must ensure all renderer object destructors are called before this
    // as this destroys the renderer queue which handles those
    // destructors
    Renderer::s_destroy();

    // this will blow away our window and context
    mp_window.reset();

    Log::s_destroy();
}

void Application::shouldQuit(Event& event)
{
    NB_UNUSED(event);
    m_active = false;
}

void Application::execute()
{
    ////////////////////////////////////////////////////////////////////////////
    // Pump all commands out that were generated from application startup
    // and layer additions
    ////////////////////////////////////////////////////////////////////////////
    Renderer::s_pumpCmds();

    auto mainLoopSw = m_swBank.newSw("Main loop");

    auto mainThreadProcessSw = m_swBank.newSw("MainThread Process");

    auto mainThreadPendRenderThreadSw = m_swBank.newSw("MainThread Pend on RenderThread");

    f64_t currentTime = core::getTime_s();
    bool  didDraw     = false;
    while (m_active)
    {
        NB_PROFILE();

        mainLoopSw->split();
        mainThreadProcessSw->split();
        ///////////////////////////
        // Loop time handling
        ///////////////////////////
        f64_t newTime  = core::getTime_s();
        f32_t loopTime = newTime - currentTime;
        currentTime    = newTime;

        // prevent spiral of death if we're running really slow
        if (loopTime >= 0.1f)
        {
            loopTime = 0.1f;
        }

        m_updateLag += loopTime;
        m_drawLag += loopTime;

        ///////////////////////////
        // Updates are fixed step
        ///////////////////////////
        bool  doUpdate        = m_updateLag >= m_updatePeriodLimit;
        u32_t updatesRequired = 0;
        if (doUpdate)
        {
            // we can run multiple updates if we're behind
            updatesRequired = m_updateLag / m_updatePeriodLimit;

            // calculate how much time we're about to process
            f32_t gameTimeProgression = (f32_t)updatesRequired * m_updatePeriodLimit;

            // remove the lag we're about to update through
            m_updateLag -= gameTimeProgression;

            // game time is a product of game updates, not renders
            m_gameTime += gameTimeProgression;
        }

        ///////////////////////////
        // Draws are limited
        ///////////////////////////
        bool doDraw = m_drawLag >= m_drawPeriodLimit;

        ///////////////////////////
        // Render thread
        ///////////////////////////
        f32_t prePendCpuProcessTime_s = mainThreadProcessSw->split();
        mainThreadPendRenderThreadSw->split();
        Renderer::s_waitForRenderThread();
        mainThreadPendRenderThreadSw->splitAndSave();
        mainThreadProcessSw->split();  // post pend

        ///////////////////////////
        // Pump events
        ///////////////////////////
        mp_window->pumpEvents();

        ///////////////////////////
        // Don't pass here if min
        ///////////////////////////
        if (mp_window->isMinimized())
        {
            continue;
        }

        /////////////////////////////////////////////////////////////////////
        // If we just drew, start the render thread
        /////////////////////////////////////////////////////////////////////
        if (didDraw)
        {
            // The render thread always runs latent, meaning it's always
            // processing the previous commands while this thread
            // (the main thread) is generating the commands for this frame.
            // It can be considered two ways, like said already or as if the
            // renderer is processing the current frame, and this thread is
            // generating commands for the next frame. Regardless, it bodes
            // consideration that updates made to objects requiring graphics
            // API calls, won't be made until the following cycle.
            // Do note that object calls (creation/deletion/resize) are handled
            // before all draw/render calls

            Renderer::s_swapAndStart();
        }

        ////////////////////////////////////////////////////////////////////////
        // Call layer update functions
        ////////////////////////////////////////////////////////////////////////
        for (u32_t i = 0; i < updatesRequired; i++)
        {
            for (auto it = m_layerDeck.begin(); it != m_layerDeck.end(); it++)
            {
                // call each update with a fixed time step
                (*it)->onUpdate(m_updatePeriodLimit);
            }
        }

        ////////////////////////////////////////////////////////////////////////
        // Call layer draw functions
        ////////////////////////////////////////////////////////////////////////
        if (doDraw)
        {
            ///////////////////////////
            // Start frame
            ///////////////////////////
            Renderer::s_startFrame();

            for (auto it = m_layerDeck.begin(); it != m_layerDeck.end(); it++)
            {
                // call each draw with how long it's been since last draw
                (*it)->onDraw(m_drawLag);
            }

            ///////////////////////////
            // Call all gui updates
            ///////////////////////////
            bool mouseButtonsDown
                = mp_window->nouseButtonDown(MouseButton::left) || mp_window->nouseButtonDown(MouseButton::right);

            SDL_CaptureMouse((mouseButtonsDown != 0) ? SDL_TRUE : SDL_FALSE);

            Application* app = this;
            Renderer::s_submit([app]() { app->guiRender(); });
            Renderer::s_submit([app]() { app->mp_guiSubsystemLayer->end(); });

            ///////////////////////////
            // End Frame
            ///////////////////////////
            Renderer::s_endFrame();

            mp_window->swapBuffers();

            didDraw   = true;
            m_drawLag = 0.0f;
        }
        else
        {
            didDraw = false;
        }

        f32_t postPendCpuProcessTime_s = mainThreadProcessSw->split();
        mainThreadProcessSw->saveSplitOverride(prePendCpuProcessTime_s + postPendCpuProcessTime_s);

        mainLoopSw->splitAndSave();
    }

    Log::coreInfo("Quitting");
}

void Application::terminate()
{
    m_active = false;
}

void Application::onEvent(Event& event)
{
    // interate over the layer deck backwards because we want layers at
    // the top of the deck to handle events first if they so choose
    // this is because when rendering layers, they are rendered bottom
    // to top so the last layer will be the layer "highest" in the scene
    for (auto it = m_layerDeck.rbegin(); it != m_layerDeck.rend(); ++it)
    {
        if (event.wasHandled())
        {
            break;
        }
        (*it)->onEvent(event);
    }

    ////////////////////////////////////////
    // Handle Application Centric Events
    ////////////////////////////////////////
    switch (event.getEventType())
    {
        case Event::Type::quit:
        {
            m_active = false;
            break;
        }
        default:
        {
            break;
        }
    }
}

void Application::insertLayer(const ref<Layer> p_layer, i32_t location)
{
    m_layerDeck.insertLayer(p_layer, location);
}

void Application::removeLayer(const ref<Layer>& p_layer)
{
    m_layerDeck.removeLayer(p_layer);
}

void Application::guiSubsystemCaptureEvents(bool capture)
{
    mp_guiSubsystemLayer->captureEvents(capture);
}

void Application::guiRender()
{
    mp_guiSubsystemLayer->begin();

    for (auto it = m_layerDeck.begin(); it != m_layerDeck.end(); it++)
    {
        (*it)->onGuiDraw(m_drawLag);
    }
}

const u8_t* Application::getKeyboardState() const
{
    return SDL_GetKeyboardState(nullptr);
}

void Application::setMenuMode(bool mode)
{
    // if mode doesn't change, don't do anything
    if (m_menuMode == mode)
    {
        return;
    }

    if (mode)
    {
        SDL_ShowCursor(SDL_ENABLE);
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }
    else
    {
        SDL_ShowCursor(SDL_DISABLE);
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    m_menuMode = mode;
}

void Application::setUpdatePeriodLimit(f32_t limit)
{
    if (limit < 0.0f)
    {
        Log::coreWarn("Update Period limit can't be less then 0.0");
        limit = 0.0f;
    }

    m_updatePeriodLimit = limit;
}

void Application::setDrawPeriodLimit(f32_t limit)
{
    if (limit < 0.0f)
    {
        Log::coreWarn("Draw Period limit can't be less then 0.0");
        limit = 0.0f;
    }
    m_drawPeriodLimit = limit;
}

};  // namespace nimbus
