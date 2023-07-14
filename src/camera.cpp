#include "camera.hpp"

#include "application.hpp"
#include "core.hpp"
#include "nmpch.hpp"

namespace nimbus
{

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_position(position),
      m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
      m_worldUp(up),
      m_yaw(yaw),
      m_pitch(pitch),
      m_speed(K_SPEED_DEFAULT),
      m_sensitivity(K_SENSITIVITY_DEFAULT),
      m_fov(K_FOV_DEFAULT),
      m_near(K_NEAR_DEFAULT),
      m_far(K_FAR_DEFAULT),
      m_is3d(true)
{
    setAspectRatio(Application::get().getWindow().getWidth(),
                   Application::get().getWindow().getHeight());

    _updateCameraVectors();

    updateView();
    updateProjection();
    updateViewProjection();
}

Camera::Camera(float left, float right, float bottom, float top)
    : m_position({0.0, 0.0, 0.0}),
      m_yaw(0.0f),
      m_orthoLeft(left),
      m_orthoRight(right),
      m_orthoBottom(bottom),
      m_orthoTop(top),
      m_is3d(false)

{
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

void Camera::processZoom(float yOffset)
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

    NM_CORE_INFO("%s\n", __func__);

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

    m_staleView           = false;
    m_staleViewProjection = true;
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

    NM_CORE_INFO("%s\n", __func__);

    if (m_is3d)
    {
        m_projection = glm::perspective(
            glm::radians(m_fov), m_aspectRatio, m_near, m_far);
    }
    else
    {
        m_projection
            = glm::ortho(m_orthoLeft,
                         (float)Application::get().getWindow().getWidth(),
                         (float)Application::get().getWindow().getHeight(),
                         m_orthoTop,
                         -1.0f,
                         1.0f);
    }

    m_staleProjection     = false;
    m_staleViewProjection = true;
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

    NM_CORE_INFO("%s\n", __func__);
    m_viewProjection = (getView() * getProjection());

    m_staleViewProjection = false;
}

glm::mat4& Camera::getViewProjection()
{
    NM_PROFILE_TRACE();

    if (m_staleViewProjection)
    {
        updateViewProjection();
    }
    return m_viewProjection;
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

void Camera::setAspectRatio(float width, float height)
{
    m_aspectRatio     = width / height;
    m_staleProjection = true;
}

}  // namespace nimbus