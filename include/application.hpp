#pragma once

#include "camera.hpp"
#include "core.hpp"
#include "event.hpp"
#include "guiLayers/guiSubsystem.hpp"
#include "layerDeck.hpp"
#include "window.hpp"

namespace nimbus
{

class Application
{
   public:
    Application(const std::string& name        = "Program",
                int               windowWidth  = 1280,
                int               windowHeight = 720,
                bool              is3d         = true);

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

    void insertLayer(Layer* layer, int32_t location = k_insertLocationHead);

    void removeLayer(Layer* layer);

    float getFrametime() const;

    const uint8_t* getKeyboardState() const;

    const glm::mat4 getProjectionMatrix(bool perspective = true) const;

    const glm::mat4 getViewMatrix() const;

    LayerDeck& getLayerDeck();

    void cameraViewUpdate(float xOffset,
                          float yOffset,
                          bool  constrainPitch = true);

    void cameraZoomUpdate(float yOffset);

    void cameraPosUpdate(Camera::Movement movement);

    Window& getWindow();

    void setMenuMode(bool mode);

    bool getMenuMode() const;

   private:
    inline static Application* sp_instance = nullptr;

    std::string         m_name;
    bool                m_is3d;
    scope<Window>       mp_window            = nullptr;
    scope<Camera>       mp_camera            = nullptr;
    scope<GuiSubsystem> mp_guiSubsystemLayer = nullptr;
    bool                m_menuMode           = false;
    volatile bool       m_Active             = true;

    LayerDeck m_layerDeck;

};

Application* createApplication();

}  // namespace nimbus