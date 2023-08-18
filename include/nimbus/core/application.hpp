#pragma once

#include "nimbus/core/common.hpp"
#include "nimbus/core/event.hpp"
#include "nimbus/core/layerDeck.hpp"
#include "nimbus/core/log.hpp"
#include "nimbus/core/window.hpp"
#include "nimbus/core/stopwatch.hpp"

#include "nimbus/guiSubsystem/guiSubsystem.hpp"
#include "nimbus/core/resourceManager.hpp"

namespace nimbus
{

class Application
{
   public:
    Application(const std::string& name = "Program", uint32_t windowWidth = 1280, uint32_t windowHeight = 720);

    virtual ~Application();

    inline static Application& s_get()
    {
        return *sp_instance;
    }

    virtual void onInit()
    {
    }

    virtual void onExit()
    {
    }

    void shouldQuit(Event& event);

    // ironically these could mean the same thing but are totally different :)
    void execute();    // main execution function
    void terminate();  // termination function if desired for testing

    void onEvent(Event& event);

    void insertLayer(const ref<Layer> p_layer, int32_t location = k_insertLocationHead);

    void removeLayer(const ref<Layer>& p_layer);

    void guiRender();

    void guiSubsystemCaptureEvents(bool capture);

    const uint8_t* getKeyboardState() const;

    void setMenuMode(bool mode);

    void setUpdatePeriodLimit(float limit);

    void setDrawPeriodLimit(float limit);

    inline bool getMenuMode() const
    {
        return m_menuMode;
    }

    inline const LayerDeck& getLayerDeck()
    {
        return m_layerDeck;
    }

    inline Window& getWindow()
    {
        return *mp_window;
    }

    inline ResourceManager& getResourceManager()
    {
        return *mp_resourceManager;
    }

    inline float getUpdateLag() const
    {
        return m_updateLag;
    }

    inline float getDrawLag() const
    {
        return m_drawLag;
    }

    inline double getGameTime() const
    {
        return m_gameTime;
    }

    inline StopWatchBank& getSwBank()
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
    inline static Application* sp_instance          = nullptr;
    scope<Window>              mp_window            = nullptr;
    scope<ResourceManager>     mp_resourceManager   = nullptr;
    ref<GuiSubsystem>          mp_guiSubsystemLayer = nullptr;
};

Application* createApplication();

}  // namespace nimbus