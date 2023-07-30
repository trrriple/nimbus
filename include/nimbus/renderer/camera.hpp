#pragma once

#include "glm.hpp"

namespace nimbus
{


class Camera
{
    inline static const float k_yaw_default         = 0.0f;
    inline static const float k_pitch_default       = 0.0f;
    inline static const float k_speed_default       = 10.0f;
    inline static const float k_sensitivity_default = 0.05f;
    inline static const float k_fov_default         = 45.0f;
    inline static const float k_zoom_default        = 1.0f;
    inline static const float k_near_default        = 0.1f;
    inline static const float k_far_default         = 300.0f;

   public:
    // Defines several possible options for camera movement.
    // Used as abstraction to stay away from window-system specific input
    // methods
    enum class Movement
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN,
    };

    struct Bounds
    {
        glm::vec4 topLeft;
        glm::vec4 topRight;
        glm::vec4 bottomLeft;
        glm::vec4 bottomRight;
    };

    // 3d camera
    Camera(const glm::vec3 position    = glm::vec3(0.0f, 0.0f, 0.0f),
           const glm::vec3 up          = glm::vec3(0.0f, 1.0f, 0.0f),
           float           yaw         = k_yaw_default,
           float           pitch       = k_pitch_default,
           float           aspectRatio = 1.0f);

    //2d camera
    Camera(float aspectRatio = 1.0f);

    // processes input received from any position movement input system.
    // Accepts input parameter in the form of camera defined ENUM
    // (to abstract it from windowing systems)
    void processPosiUpdate(Movement direction, float deltaTime);

    // processes input received from a view update.
    void processViewUpdate(float xOffset,
                           float yOffset,
                           bool  constrainPitch = true);

    // processes input received from a canera zoom event
    void processFov(float yOffset);

    void       updateView();
    glm::mat4& getView();

    void       updateProjection();
    glm::mat4& getProjection();

    void       updateViewProjection();
    glm::mat4& getViewProjection();

    Bounds& getVisibleWorldBounds();

    void setAspectRatio(float aspectRatio);

    float getAspectRatio() const
    {
        return m_aspectRatio;
    }

    void setPosition(const glm::vec3& position);

    void setZoom(float zoom);

    const glm::vec3& getPosition() const
    {
        return m_position;
    }

    void setSpeed(float speed);

    float getSpeed() const
    {
        return m_speed;
    }

    void  setSensitivity(float sensitivity);

    float getSensitivity() const
    {
        return m_sensitivity;
    }

    float getYaw() const
    {
        return m_yaw;
    }

    float getPitch() const
    {
        return m_pitch;
    }

   private:
    glm::mat4 m_projection;
    glm::mat4 m_view;
    glm::mat4 m_viewProjection;
    Bounds    m_worldBounds;

    bool m_staleProjection;
    bool m_staleView;
    bool m_staleWorldBounds;

    // location
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    // euler angles
    float m_yaw;
    float m_pitch;

    // tuning values
    float m_speed;
    float m_sensitivity;
    float m_fov;
    float m_zoom;
    float m_aspectRatio;

    float m_near;
    float m_far;

    // ortho specific parameters (2d)
    float m_orthoLeft;
    float m_orthoRight;
    float m_orthoBottom;
    float m_orthoTop;

    bool m_is3d;

    // calculates the front vector from the Camera's (updated) Euler Angles
    void _updateCameraVectors();
    float _getAspectRatio();
};

}  // namespace nimbus