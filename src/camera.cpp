#include "nmpch.hpp"
#include "core.hpp"

#include "camera.hpp"

namespace nimbus
{

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
      m_speed(K_SPEED_DEFAULT),
      m_sensitivity(K_SENSITIVITY_DEFAULT),
      m_fov(K_FOV_DEFAULT)
{
    m_position = position;
    m_worldUp  = up;
    m_yaw      = yaw;
    m_pitch    = pitch;
    updateCameraVectors();
}

Camera::Camera(float posX,
               float posY,
               float posZ,
               float upX,
               float upY,
               float upZ,
               float yaw,
               float pitch)
    : m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
      m_speed(K_SPEED_DEFAULT),
      m_sensitivity(K_SENSITIVITY_DEFAULT),
      m_fov(K_FOV_DEFAULT)
{
    m_position = glm::vec3(posX, posY, posZ);
    m_worldUp  = glm::vec3(upX, upY, upZ);
    m_yaw      = yaw;
    m_pitch    = pitch;
    updateCameraVectors();
}

void Camera::processPosUpd(Movement direction, float deltaTime)
{
    float velocity = m_speed * deltaTime;
    switch (direction)
    {
        case (Movement::FORWARD):
        {
            m_position += m_front * velocity;
            break;
        }
        case (Movement::BACKWARD):
        {
            m_position -= m_front * velocity;
            break;
        }
        case (Movement::LEFT):
        {
            m_position -= m_right * velocity;
            break;
        }
        case (Movement::RIGHT):
        {
            m_position += m_right * velocity;
            break;
        }
        case (Movement::UP):
        {
            m_position += m_up * velocity;
            break;
        }
        case (Movement::DOWN):
        {
            m_position -= m_up * velocity;
            break;
        }
        default:
            break;
    }
}

void Camera::processViewUpdate(float xOffset,
                               float yOffset,
                               bool  constrainPitch)
{
    xOffset *= m_sensitivity;
    yOffset *= m_sensitivity;

    m_yaw += xOffset;
    m_pitch += yOffset;

    // make sure that when pitch is out of bounds, screen doesn't get
    // flipped
    if (constrainPitch)
    {
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::processZoom(float yOffset)
{
    m_fov -= (float)yOffset;
    if (m_fov < 1.0f)
        m_fov = 1.0f;
    if (m_fov > 45.0f)
        m_fov = 45.0f;
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    m_right = glm::normalize(
        glm::cross(m_front,
                   m_worldUp));  // normalize the vectors, because their length
                                 // gets closer to 0 the more you look up or
                                 // down which results in slower movement.
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

}  // namespace nimbus