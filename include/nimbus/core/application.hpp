#pragma once

#include "nimbus/core/common.hpp"
#include "nimbus/core/event.hpp"
#include "nimbus/core/layerDeck.hpp"
#include "nimbus/core/log.hpp"
#include "nimbus/core/window.hpp"

#include "nimbus/guiLayers/guiSubsystem.hpp"

namespace nimbus
{

class Application
{
   public:
    Application(const std::string& name         = "Program",
                uint32_t           windowWidth  = 1280,
                uint32_t           windowHeight = 720);

    virtual ~Application() = default;

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

    void insertLayer(const ref<Layer>& p_layer,
                     int32_t           location = k_insertLocationHead);

    void removeLayer(const ref<Layer>& p_layer);

    void guiSubsystemCaptureEvents(bool capture);

    const uint8_t* getKeyboardState() const;

    void setMenuMode(bool mode);

    void setUpdatePeriodLimit(float limit);

    void setDrawPeriodLimit(float limit);

    bool getMenuMode() const
    {
        return m_menuMode;
    }

    const LayerDeck& getLayerDeck()
    {
        return m_layerDeck;
    }

    Window& getWindow()
    {
        return *mp_window;
    }

    float getUpdateLag() const
    {
        return m_updateLag;
    }

    float getDrawLag() const
    {
        return m_drawLag;
    }

    double getGameTime() const
    {
        return m_gameTime;
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
    bool          m_menuMode  = false;
    volatile bool m_Active    = true;
    double        m_gameTime  = 0.0f;
    float         m_updateLag = 0.0f;
    float         m_drawLag   = 0.0f;
    LayerDeck     m_layerDeck;

    ///////////////////////////
    // References
    ///////////////////////////
    inline static Application* sp_instance          = nullptr;
    scope<Window>              mp_window            = nullptr;
    ref<GuiSubsystem>          mp_guiSubsystemLayer = nullptr;
};

Application* createApplication();

}  // namespace nimbus