#include "application.hpp"

#include "core.hpp"
#include "guiLayers/engineGui.hpp"
#include "guiLayers/guiSubsystem.hpp"
#include "nmpch.hpp"
#include "platform/rendererApi.hpp"
#include "renderer/renderer.hpp"

namespace nimbus
{

Application::Application(const std::string& name,
                         uint32_t           windowWidth,
                         uint32_t           windowHeight,
                         bool               is3d)
    : m_name(name), m_is3d(is3d)
{
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

    NM_CORE_INFO("------------------------------------------\n");
    NM_CORE_INFO("----- Nimbus Engine Application Init -----\n");
    NM_CORE_INFO("------------------------------------------\n");

    NM_CORE_ASSERT(!sp_instance, "Application should only be created once!\n");

    sp_instance = this;

    mp_window = makeScope<Window>(m_name, windowWidth, windowHeight);
    mp_window->graphicsContextInit();

    Renderer::init();

    mp_window->setEventCallback(
        std::bind(&Application::onEvent, this, std::placeholders::_1));

    mp_window->setExitCallback(
        std::bind(&Application::shouldQuit, this, std::placeholders::_1));

    if (m_is3d)
    {
        RendererApi::setDepthTest(true);
        mp_camera = makeScope<Camera>(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else
    {
        RendererApi::setDepthTest(false);
        mp_camera = makeScope<Camera>(0.0f,
                                      (float)mp_window->getWidth(),
                                      (float)mp_window->getHeight(),
                                      0.0f);
    }

    mp_guiSubsystemLayer = makeRef<GuiSubsystem>();
    insertLayer(mp_guiSubsystemLayer);
    insertLayer(makeRef<EngineGui>());
}

void Application::shouldQuit(Event& event)
{
    UNUSED(event);
    m_Active = false;
}

void Application::execute()
{
    while (m_Active)
    {
        NM_PROFILE();

        RendererApi::clear();

        ////////////////////////////////////////////////////////////////////////
        // Call layer update functions
        ////////////////////////////////////////////////////////////////////////
        for (auto it = m_layerDeck.begin(); it != m_layerDeck.end(); ++it)
        {
            (*it)->onUpdate();
        }

        ////////////////////////////////////////////////////////////////////////
        // Call layer GUI update functions
        ////////////////////////////////////////////////////////////////////////
        mp_guiSubsystemLayer->begin();

        for (auto it = m_layerDeck.begin(); it != m_layerDeck.end(); ++it)
        {
            (*it)->onGuiUpdate();
        }

        mp_guiSubsystemLayer->end();

        ////////////////////////////////////////////////////////////////////////
        // Call window update function (events polled and buffers swapped)
        ////////////////////////////////////////////////////////////////////////
        mp_window->onUpdate();
    }

    NM_CORE_INFO("Quitting\n");
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

float Application::getFrametime() const
{
    return mp_window->m_tFrame_s;
}

const uint8_t* Application::getKeyboardState() const
{
    return SDL_GetKeyboardState(nullptr);
}

LayerDeck& Application::getLayerDeck()
{
    return m_layerDeck;
}

Window& Application::getWindow()
{
    return *mp_window;
}

Camera& Application::getCamera()
{
    return *mp_camera;
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

bool Application::getMenuMode() const
{
    return m_menuMode;
}

};  // namespace nimbus
