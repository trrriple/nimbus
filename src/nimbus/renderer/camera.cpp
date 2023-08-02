#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/renderer/camera.hpp"

#include "nimbus/core/application.hpp"

namespace nimbus
{

Camera::Camera(const glm::vec3 position,
               const glm::vec3 up,
               float           yaw,
               float           pitch,
               float           aspectRatio)
    : m_position(position),
      m_front({0.0f, 0.0f, -1.0f}),
      m_worldUp(up),
      m_yaw(yaw),
      m_pitch(pitch),
      m_speed(k_speed_default),
      m_sensitivity(k_sensitivity_default),
      m_fov(k_fov_default),
      m_aspectRatio(aspectRatio),
      m_near(k_near_default),
      m_far(k_far_default),
      m_is3d(true)
{
    _updateCameraVectors();

    updateView();
    updateProjection();
    updateViewProjection();
}

Camera::Camera(float aspectRatio)
    : m_position({0.0f, 0.0f, 0.0f}),
      m_front({0.0f, 0.0f, -1.0f}),
      m_worldUp({0.0f, 1.0f, 0.0f}),
      m_yaw(k_yaw_default),
      m_pitch(k_pitch_default),
      m_speed(k_speed_default),
      m_sensitivity(k_sensitivity_default),
      m_zoom(k_zoom_default),
      m_aspectRatio(aspectRatio),
      m_is3d(false)
{
    _updateCameraVectors();

    updateView();
    updateProjection();
    updateViewProjection();
}

void Camera::processPosiUpdate(Movement direction, float deltaTime)
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

    m_staleView = true;
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
    _updateCameraVectors();

    m_staleView = true;
}

void Camera::processFov(float yOffset)
{
    NM_PROFILE_TRACE();

    m_fov -= (float)yOffset;
    if (m_fov < 1.0f)
        m_fov = 1.0f;
    if (m_fov > 45.0f)
        m_fov = 45.0f;

    m_staleProjection = true;
}

void Camera::updateView()
{
    NM_PROFILE_DETAIL();

    if (m_is3d)
    {
        m_view = glm::lookAt(m_position, m_position + m_front, m_up);
    }
    else
    {
        glm::mat4 transform
            = glm::translate(glm::mat4(1.0f), m_position)
              * glm::rotate(
                  glm::mat4(1.0f), glm::radians(m_yaw), glm::vec3(0, 0, 1));

        m_view = glm::inverse(transform);
    }

    m_staleView = false;
}

glm::mat4& Camera::getView()
{
    NM_PROFILE_TRACE();

    if (m_staleView)
    {
        updateView();
    }

    return m_view;
}

void Camera::updateProjection()
{
    NM_PROFILE_DETAIL();

    if (m_is3d)
    {
        m_projection = glm::perspective(
            glm::radians(m_fov), m_aspectRatio, m_near, m_far);
    }
    else
    {
        m_projection = glm::ortho(-m_aspectRatio * m_zoom,
                                  m_aspectRatio * m_zoom,
                                  -m_zoom,
                                  m_zoom,
                                  -1.0f,
                                  1.0f);
    }

    m_staleProjection = false;
}

glm::mat4& Camera::getProjection()
{
    NM_PROFILE_TRACE();

    if (m_staleProjection)
    {
        updateProjection();
    }

    return m_projection;
}

void Camera::updateViewProjection()
{
    NM_PROFILE_DETAIL();

    m_viewProjection = getProjection() * getView();

    m_staleWorldBounds = true;
}

glm::mat4& Camera::getViewProjection()
{
    NM_PROFILE_TRACE();

    if (m_staleView || m_staleProjection)
    {
        updateViewProjection();
    }
    return m_viewProjection;
}

Camera::Bounds& Camera::getVisibleWorldBounds()
{
    // Calculate the inverse view-projection matrix

    if (m_staleWorldBounds)
    {
        glm::mat4 inverseViewProjectionMatrix = glm::inverse(m_viewProjection);

        // Corner positions in NDC
        glm::vec4 topLeftNDC(-1.0f, 1.0f, 0.0f, 1.0f);
        glm::vec4 topRightNDC(1.0f, 1.0f, 0.0f, 1.0f);
        glm::vec4 bottomLeftNDC(-1.0f, -1.0f, 0.0f, 1.0f);
        glm::vec4 bottomRightNDC(1.0f, -1.0f, 0.0f, 1.0f);

        // Transform NDC coordinates to world coordinates using the inverse
        // view-projection matrix
        m_worldBounds.topLeft    = inverseViewProjectionMatrix * topLeftNDC;
        m_worldBounds.topRight   = inverseViewProjectionMatrix * topRightNDC;
        m_worldBounds.bottomLeft = inverseViewProjectionMatrix * bottomLeftNDC;
        m_worldBounds.bottomRight
            = inverseViewProjectionMatrix * bottomRightNDC;

        // Convert the homogeneous coordinates to 3D coordinates
        m_worldBounds.topLeft /= m_worldBounds.topLeft.w;
        m_worldBounds.topRight /= m_worldBounds.topRight.w;
        m_worldBounds.bottomLeft /= m_worldBounds.bottomLeft.w;
        m_worldBounds.bottomRight /= m_worldBounds.bottomRight.w;

        m_staleWorldBounds = false;
    }

    return m_worldBounds;
}

void Camera::_updateCameraVectors()
{
    NM_PROFILE_DETAIL();

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

void Camera::setAspectRatio(float aspectRatio)
{
    m_aspectRatio     = aspectRatio;
    m_staleProjection = true;
}

void Camera::setPosition(const glm::vec3& position)
{
    m_position  = position;
    m_staleView = true;
}

void Camera::setZoom(float zoom)
{
    m_zoom            = zoom;
    m_staleProjection = true;
}

void Camera::setSpeed(float speed)
{
    m_speed = speed;
}

void Camera::setSensitivity(float sensitivity)
{
    m_sensitivity = sensitivity;
}

}  // namespace nimbus