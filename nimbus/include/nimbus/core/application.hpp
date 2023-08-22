#pragma once

#include "nimbus/core/common.hpp"
#include "nimbus/core/event.hpp"
#include "nimbus/core/layerDeck.hpp"
#include "nimbus/core/log.hpp"
#include "nimbus/core/window.hpp"
#include "nimbus/core/stopwatch.hpp"

#include "nimbus/guiSubsystem/guiSubsystem.hpp"
#include "nimbus/core/resourceManager.hpp"

#include "nimbus/script/scriptEngine.hpp"

namespace nimbus
{

class NIMBUS_API Application
{
   public:
    Application(const std::string& name = "Program", u32_t windowWidth = 1280, u32_t windowHeight = 720);

    virtual ~Application();

    static Application& s_get()
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

    void insertLayer(const ref<Layer> p_layer, i32_t location = k_insertLocationHead);

    void removeLayer(const ref<Layer>& p_layer);

    void guiRender();

    void guiSubsystemCaptureEvents(bool capture);

    void* getGuiContext()
    {
        return mp_guiSubsystemLayer->getGuiContext();
    }

    const u8_t* getKeyboardState() const;

    void setMenuMode(bool mode);

    void setUpdatePeriodLimit(f32_t limit);

    void setDrawPeriodLimit(f32_t limit);

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

    inline f32_t getUpdateLag() const
    {
        return m_updateLag;
    }

    inline f32_t getDrawLag() const
    {
        return m_drawLag;
    }

    inline f64_t getGameTime() const
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
    f32_t       m_updatePeriodLimit = 0.00834f; // 120 hz
    f32_t       m_drawPeriodLimit   = 0.00834f; // 120 hz

    ///////////////////////////
    // State
    ///////////////////////////
    LayerDeck     m_layerDeck;
    bool          m_menuMode  = false;
    volatile bool m_active    = true;
    f64_t         m_gameTime  = 0.0f;
    f32_t         m_updateLag = 0.0f;
    f32_t         m_drawLag   = 0.0f;
    StopWatchBank m_swBank;

    ///////////////////////////
    // References
    ///////////////////////////
    static Application*    sp_instance;
    scope<Window>          mp_window            = nullptr;
    scope<ResourceManager> mp_resourceManager   = nullptr;
    ref<GuiSubsystem>      mp_guiSubsystemLayer = nullptr;
};

Application* createApplication();

}  // namespace nimbus