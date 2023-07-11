#pragma once

#include "core.hpp"

namespace nimbus
{

const float K_YAW_DEFAULT         = 0.0f;
const float K_PITCH_DEFAULT       = 0.0f;
const float K_SPEED_DEFAULT       = 10.0f;
const float K_SENSITIVITY_DEFAULT = 0.05f;
const float K_FOV_DEFAULT         = 45.0f;

class Camera
{
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

    // attributes
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

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f),
           float     yaw      = K_YAW_DEFAULT,
           float     pitch    = K_PITCH_DEFAULT);

    // constructor with scalar values
    Camera(float posX,
           float posY,
           float posZ,
           float upX,
           float upY,
           float upZ,
           float yaw,
           float pitch);

    // processes input received from any position movement input system.
    // Accepts input parameter in the form of camera defined ENUM
    // (to abstract it from windowing systems)
    void processPosUpd(Movement direction, float deltaTime);

    // processes input received from a view update.
    void processViewUpdate(float xOffset,
                           float yOffset,
                           bool  constrainPitch = true);

    // processes input received from a canera zoom event
    void processZoom(float yOffset);

    glm::mat4 getViewMatrix();

   private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
};

}  // namespace nimbus