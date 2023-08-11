#pragma once

#include "nimbus/core/common.hpp"
#include "nimbus/core/event.hpp"
#include "nimbus/core/layerDeck.hpp"
#include "nimbus/core/log.hpp"
#include "nimbus/core/window.hpp"
#include "nimbus/core/stopwatch.hpp"

#include "nimbus/guiLayers/guiSubsystem.hpp"
#include "nimbus/core/resourceManager.hpp"

namespace nimbus
{

class Application
{
   public:
    Application(const std::string& name         = "Program",
                uint32_t           windowWidth  = 1280,
                uint32_t           windowHeight = 720) noexcept;

    virtual ~Application() noexcept;

    inline static Application& s_get() noexcept
    {
        return *sp_instance;
    }

    virtual void onInit() noexcept
    {
    }

    virtual void onExit() noexcept
    {
    }

    void shouldQuit(Event& event) noexcept;

    // ironically these could mean the same thing but are totally different :)
    void execute() noexcept;    // main execution function
    void terminate() noexcept;  // termination function if desired for testing

    void onEvent(Event& event) noexcept;

    void insertLayer(const ref<Layer>& p_layer,
                     int32_t location = k_insertLocationHead) noexcept;

    void removeLayer(const ref<Layer>& p_layer) noexcept;

    void guiRender() noexcept;

    void guiSubsystemCaptureEvents(bool capture) noexcept;

    const uint8_t* getKeyboardState() const noexcept;

    void setMenuMode(bool mode) noexcept;

    void setUpdatePeriodLimit(float limit) noexcept;

    void setDrawPeriodLimit(float limit) noexcept;

    inline bool getMenuMode() const noexcept
    {
        return m_menuMode;
    }

    inline const LayerDeck& getLayerDeck() noexcept
    {
        return m_layerDeck;
    }

    inline Window& getWindow() noexcept
    {
        return *mp_window;
    }

    inline ResourceManager& getResourceManager() noexcept
    {
        return *mp_resourceManager;
    }

    inline float getUpdateLag() const noexcept
    {
        return m_updateLag;
    }

    inline float getDrawLag() const noexcept
    {
        return m_drawLag;
    }

    inline double getGameTime() const noexcept
    {
        return m_gameTime;
    }

    inline StopWatchBank& getSwBank() noexcept
    {
       return m_swBank;
    } 

   private:
    ///////////////////////////
    // Parameters
    ///////////////////////////
    std::string m_name;
    float       m_updatePeriodLimit = 0.0167f;
    float       m_drawPeriodLimit   = 0.0167f;

    ///////////////////////////
    // State
    ///////////////////////////
    LayerDeck     m_layerDeck;
    bool          m_menuMode  = false;
    volatile bool m_active    = true;
    double        m_gameTime  = 0.0f;
    float         m_updateLag = 0.0f;
    float         m_drawLag   = 0.0f;
    StopWatchBank m_swBank;

    ///////////////////////////
    // References
    ///////////////////////////
    inline static Application* sp_instance        = nullptr;
    scope<Window>              mp_window          = nullptr;
    scope<ResourceManager>     mp_resourceManager = nullptr;

    ref<GuiSubsystem> mp_guiSubsystemLayer = nullptr;
};

Application* createApplication();

}  // namespace nimbus