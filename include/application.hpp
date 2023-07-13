#pragma once

#include "camera.hpp"
#include "core.hpp"
#include "event.hpp"
#include "layerDeck.hpp"
#include "renderer/renderer.hpp"
#include "window.hpp"

namespace nimbus
{
typedef std::function<void()> nbCallback_t;


class Application
{
   public:
    Application(const std::string& name        = "Program",
                int               windowWidth  = 1280,
                int               windowHeight = 720,
                bool              is3d         = true);

    virtual ~Application();

    static Application& get()
    {
        return *sp_instance;
    }

    virtual void onInit();

    virtual void onExit();

    bool shouldQuit() const;

    void execute();

    void onEvent(Event& event);

    void insertLayer(Layer* layer, int32_t location = k_insertLocationHead);

    void removeLayer(Layer* layer);

    void addGuiCallback(nbCallback_t p_func);

    float getFrametime() const;

    const uint8_t* getKeyboardState() const;

    const glm::mat4 getProjectionMatrix(bool perspective = true) const;

    const glm::mat4 getViewMatrix() const;

    void cameraViewUpdate(float     xOffset,
                          float     yOffset,
                          GLboolean constrainPitch = true);

    void cameraZoomUpdate(float yOffset);

    void cameraPosUpdate(Camera::Movement movement);

    Window& getWindow();

    void setMenuMode(bool mode);

    bool getMenuMode() const;

   private:
    inline static Application* sp_instance = nullptr;

    std::string               m_name;
    std::unique_ptr<Window>   mp_window   = nullptr;
    std::unique_ptr<Camera>   mp_camera   = nullptr;
    bool                      m_menuMode  = false;
    volatile bool             m_Active    = true;

    LayerDeck m_layerDeck;

    std::vector<nbCallback_t> m_guiCallbacks;

    void _initGui();

    void _killGui();

    void _render();

    void _renderStatsDisplay();

    void _cameraMenu();
};

Application* createApplication();

}  // namespace nimbus