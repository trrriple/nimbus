#pragma once

#include "core.hpp"
#include "camera.hpp"
#include "renderer.hpp"

namespace nimbus
{
typedef std::function<void()>                      nbCallback_t;
typedef std::function<void(SDL_WindowEvent&)>      nbWindowEvtCallback_t;
typedef std::function<void(SDL_MouseMotionEvent&)> nbMouseMotionEvtCallback_t;
typedef std::function<void(SDL_MouseWheelEvent&)>  nbMouseWheelEvtCallback_t;
typedef std::function<void(SDL_MouseButtonEvent&)> nbMouseButtonEvtCallback_t;

class Application
{
   public:
    Application(const std::string name         = "Program",
                int               screenWidth  = 1280,
                int               screenHeight = 720,
                bool              is3d         = true);

    virtual ~Application();

    virtual void onInit();

    virtual void onExit();

    bool shouldQuit() const;

    void execute();

    void addGuiCallback(nbCallback_t p_func);

    void addRenderCallback(nbCallback_t p_func);

    void addUpdateCallback(nbCallback_t p_func);

    void setEventCallback(nbWindowEvtCallback_t p_func);

    void setEventCallback(nbMouseMotionEvtCallback_t p_func);

    void setEventCallback(nbMouseWheelEvtCallback_t p_func);

    void setEventCallback(nbMouseButtonEvtCallback_t p_func);

    SDL_Window* getWindow() const;

    float getFrametime() const;

    const uint8_t* getKeyboardState() const;

    const glm::mat4 getProjectionMatrix(bool perspective = true) const;

    const glm::mat4 getViewMatrix() const;

    void cameraViewUpdate(float     xOffset,
                          float     yOffset,
                          GLboolean constrainPitch = true);

    void cameraZoomUpdate(float yOffset);

    void cameraPosUpdate(Camera::Movement movement);

    int getScreenHeight() const;

    int getScreenWidth() const;

    void setMenuMode(bool mode);

    bool getMenuMode() const;

   private:
    std::string               m_name;
    int                       m_screenWidth;
    int                       m_screenHeight;
    SDL_Window*               mp_window   = nullptr;
    std::unique_ptr<Renderer> mp_renderer = nullptr;
    std::unique_ptr<Camera>   mp_camera   = nullptr;
    bool                      m_menuMode  = false;
    volatile bool             m_quit      = false;

    std::vector<nbCallback_t> m_guiCallbacks;
    std::vector<nbCallback_t> m_renderCallbacks;
    std::vector<nbCallback_t> m_updateCallbacks;

    nbWindowEvtCallback_t      m_windowEvtCallback      = nullptr;
    nbMouseMotionEvtCallback_t m_mouseMotionEvtCallback = nullptr;
    nbMouseWheelEvtCallback_t  m_mouseWheelEvtCallback  = nullptr;
    nbMouseButtonEvtCallback_t m_mouseButtonEvtCallback = nullptr;

    void _initOsIntrf(const std::string windowCaption);

    void _killOsIntrf();

    void _initGui();

    void _killGui();

    void _render();

    void _onUpdate();

    void _processEvents();

    void _processWindowEvents(SDL_WindowEvent& evt);

    void _renderStatsDisplay();

    void _cameraMenu();
};

Application* createApplication();

}  // namespace nimbus