#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/core/application.hpp"

#include "nimbus/guiLayers/guiSubsystem.hpp"
#include "nimbus/renderer/graphicsApi.hpp"
#include "nimbus/renderer/renderer.hpp"
#include "nimbus/renderer/renderer2D.hpp"

#include "nimbus/renderer/font.hpp"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"

namespace nimbus
{

Application::Application(const std::string& name,
                         uint32_t           windowWidth,
                         uint32_t           windowHeight)
    : m_name(name)
{
    NM_CORE_ASSERT(!sp_instance, "Application should only be created once!\n");

    sp_instance = this;

    mp_window = genScope<Window>(m_name, windowWidth, windowHeight);
    Log::init();

    Log::coreInfo("------------------------------------------");
    Log::coreInfo("----- Nimbus Engine Application Init -----");
    Log::coreInfo("------------------------------------------");

    mp_window->graphicsContextInit();

    mp_window->setEventCallback(
        std::bind(&Application::onEvent, this, std::placeholders::_1));

    mp_window->setExitCallback(
        std::bind(&Application::shouldQuit, this, std::placeholders::_1));

 
    mp_guiSubsystemLayer = ref<GuiSubsystem>::gen();
    insertLayer(mp_guiSubsystemLayer);

    // TODO put back
    SDL_GL_MakeCurrent(static_cast<SDL_Window*>(mp_window->getOsWindow()),
                       nullptr);

    Renderer::s_init(mp_window->getOsWindow(), mp_window->getContext());
    Renderer2D::s_init();

    // TODO remove hack
    std::promise<void> renderDonePromise;
    std::future<void>  renderDoneFuture = renderDonePromise.get_future();

    // TODO better solution
    SDL_Window* p_window = static_cast<SDL_Window*>(mp_window->getOsWindow());
    Renderer::s_submit(
        [p_window, &renderDonePromise]()
        {
            SDL_GL_SwapWindow(p_window);
            renderDonePromise.set_value();
        });
    
    // Renderer::s_processHook();

    renderDoneFuture.wait();
}

Application::~Application()
{
    Renderer2D::s_destroy();
    Renderer::s_destroy();
}

void Application::shouldQuit(Event& event)
{
    NM_UNUSED(event);
    m_Active = false;
}

void Application::execute()
{
    double currentTime = core::getTime_s();
    
    while (m_Active)
    {
        NM_PROFILE();

        ///////////////////////////
        // Loop time handling
        ///////////////////////////
        double newTime  = core::getTime_s();
        float  loopTime = newTime - currentTime;
        currentTime     = newTime;

        // prevent spiral of death if we're running really slow
        if(loopTime >= 0.1f)
        {
            loopTime = 0.1f;
        }

        m_updateLag += loopTime;
        m_drawLag   += loopTime;

        ///////////////////////////
        // Updates are fixed step
        ///////////////////////////
        bool     doUpdate        = m_updateLag >= m_updatePeriodLimit;
        uint32_t updatesRequired = 0;
        if (doUpdate)
        {
            // we can run multiple updates if we're behind
            updatesRequired = m_updateLag / m_updatePeriodLimit;
            
            // calculate how much time we're about to process
            float gameTimeProgression
                = (float)updatesRequired * m_updatePeriodLimit;
            
            // remove the lag we're about to update through
            m_updateLag -= gameTimeProgression;

            // game time is a product of game updates, not renders
            m_gameTime += gameTimeProgression;
        }

        ///////////////////////////
        // Draws are limited
        ///////////////////////////
        bool doDraw = m_drawLag >= m_drawPeriodLimit;

        ////////////////////////////////////////////////////////////////////////
        // Call layer update functions
        ////////////////////////////////////////////////////////////////////////
        for (uint32_t i = 0; i < updatesRequired; i++)
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

            GraphicsApi::clear();
            for (auto it = m_layerDeck.begin(); it != m_layerDeck.end(); it++)
            {
                // call each draw with how long it's been since last draw
                (*it)->onDraw(m_drawLag);
            }

            ///////////////////////////
            // Call all gui updates
            ///////////////////////////
            bool mouseButtonsDown
                = mp_window->mouseButtonPressed(MouseButton::LEFT)
                  || mp_window->mouseButtonPressed(MouseButton::RIGHT);

            SDL_CaptureMouse((mouseButtonsDown != 0) ? SDL_TRUE : SDL_FALSE);

            Application* app = this;
            Renderer::s_submit([app]() { app->guiRender(); });
            Renderer::s_submit([app]() { app->mp_guiSubsystemLayer->end(); });

            std::promise<void> renderDonePromise;
            std::future<void> renderDoneFuture = renderDonePromise.get_future();

            // TODO better solution            
            SDL_Window* p_window
                = static_cast<SDL_Window*>(mp_window->getOsWindow());
            Renderer::s_submit(
                [p_window, &renderDonePromise]()
                {
                    SDL_GL_SwapWindow(p_window);
                    renderDonePromise.set_value();
                });

            // Renderer::s_processHook();
            renderDoneFuture.wait();


            ////////////////////////////////////////////////////////////////////
            // Call window update function (events polled and buffers swapped)
            ////////////////////////////////////////////////////////////////////
            mp_window->onUpdate();
            m_drawLag = 0.0f;
        }
    }

    Log::coreInfo("Quitting");
}

void Application::terminate()
{
    m_Active = false;
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
        case Event::Type::QUIT:
        {
            m_Active = false;
            break;
        }
        default:
        {
            break;
        }
    }
}

void Application::insertLayer(const ref<Layer>& p_layer, int32_t location)
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
        (*it)->onGuiUpdate(m_drawLag);
    }
}

const uint8_t* Application::getKeyboardState() const
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

void Application::setUpdatePeriodLimit(float limit)
{
    if (limit < 0.0f)
    {
        Log::coreWarn("Update Period limit can't be less then 0.0");
        limit = 0.0f;
    }

    m_updatePeriodLimit = limit;
}

void Application::setDrawPeriodLimit(float limit)
{
    if (limit < 0.0f)
    {
        Log::coreWarn("Draw Period limit can't be less then 0.0");
        limit = 0.0f;
    }
    m_drawPeriodLimit = limit;
}

};  // namespace nimbus
