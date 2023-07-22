#pragma once

#include "camera.hpp"
#include "common.hpp"
#include "event.hpp"
#include "guiLayers/guiSubsystem.hpp"
#include "layerDeck.hpp"
#include "window.hpp"
#include "log.hpp"

namespace nimbus
{

class Application
{
   public:
    Application(const std::string& name         = "Program",
                uint32_t           windowWidth  = 1280,
                uint32_t           windowHeight = 720,
                bool               is3d         = true);

    virtual ~Application() = default;

    static Application& get()
    {
        return *sp_instance;
    }

    virtual void onInit() {}

    virtual void onExit() {}

    void shouldQuit(Event& event);

    void execute();

    void onEvent(Event& event);

    void insertLayer(const ref<Layer>& p_layer,
                     int32_t           location = k_insertLocationHead);

    void removeLayer(const ref<Layer>& p_layer);

    void guiSubsystemCaptureEvents(bool capture);

    float getFrametime() const;

    const uint8_t* getKeyboardState() const;

    LayerDeck& getLayerDeck(); 

    Window& getWindow();

    Camera& getCamera();

    void setMenuMode(bool mode);

    bool getMenuMode() const;

    const Log& getAppLog() const;

    const Log& getCoreLog() const;

    void kill();

   private:
    ///////////////////////////
    // Parameters
    ///////////////////////////
    std::string m_name;
    bool        m_is3d;

    ///////////////////////////
    // State
    ///////////////////////////
    bool          m_menuMode = false;
    volatile bool m_Active   = true;
    LayerDeck     m_layerDeck;

    ///////////////////////////
    // References
    ///////////////////////////
    inline static Application* sp_instance          = nullptr;
    scope<Window>              mp_window            = nullptr;
    scope<Camera>              mp_camera            = nullptr;
    ref<GuiSubsystem>          mp_guiSubsystemLayer = nullptr;

};

Application* createApplication();

}  // namespace nimbus