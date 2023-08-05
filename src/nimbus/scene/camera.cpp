#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/scene/camera.hpp"

#include "glm.hpp"

namespace nimbus
{

Camera::Camera(Type type) : m_type(type)
{
    _updateCameraVectors();
}

void Camera::setType(Type type)
{
    m_type             = type;
    m_staleView        = true;
    m_staleProjection  = true;
    m_staleWorldBounds = true;
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

void Camera::processViewUpdate(const glm::vec2& offset, bool constrainPitch)
{
    m_yaw += offset.x * m_sensitivity;
    m_pitch += offset.y * m_sensitivity;

    // clamp yaw so we don't start losing precision
    if(m_yaw > 360.0f || m_yaw < -360.0f)
    {
        m_yaw = 0.0f;
    }

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

void Camera::processZoom(float offset)
{
    NM_PROFILE_TRACE();
    const float zoomScale = 0.1;

    if (m_type == Type::PERSPECTIVE)
    {
        m_fov -= offset;
        if (m_fov < 1.0f)
            m_fov = 1.0f;
        if (m_fov > 110.0f)
            m_fov = 110.0f;
    }
    else
    {
        m_zoom -= offset * zoomScale;
        if (m_zoom <= 0.1)
        {
            m_zoom = 0.1;
        }
    }

    m_staleProjection = true;
}

glm::mat4& Camera::getView()
{
    NM_PROFILE_DETAIL();

    if (m_staleView)
    {
        if (m_type == Type::PERSPECTIVE)
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

    return m_view;
}

glm::mat4& Camera::getProjection()
{
    NM_PROFILE_DETAIL();

    if (m_staleProjection)
    {
        if (m_type == Type::PERSPECTIVE)
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
                                      m_orthoNear,
                                      m_orthoFar);
        }

        m_staleProjection = false;
    }

    return m_projection;
}

glm::mat4& Camera::getViewProjection()
{
    NM_PROFILE_DETAIL();

    if (m_staleView || m_staleProjection)
    {
        m_viewProjection = getProjection() * getView();

        m_staleWorldBounds = true;
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

void Camera::setNearClip(float near)
{
    if (m_type == Type::PERSPECTIVE)
    {
        m_near = near;
    }
    else
    {
        m_orthoNear = near;
    }

    m_staleProjection = true;
}


void Camera::setFarClip(float far)
{
    if (m_type == Type::PERSPECTIVE)
    {
        m_far = far;
    }
    else
    {
        m_orthoFar = far;
    }

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

void Camera::setFov(float fov)
{
    m_fov             = fov;
    m_staleProjection = true;
}

void Camera::setSpeed(float speed)
{
    m_speed = speed;
}

void Camera::setYaw(float yaw)
{
    m_yaw = yaw;
    _updateCameraVectors();

    m_staleView = true;
}

void Camera::setPitch(float pitch)
{
    m_pitch = pitch;
    _updateCameraVectors();

    m_staleView = true;
}

void Camera::setSensitivity(float sensitivity)
{
    m_sensitivity = sensitivity;
}

}  // namespace nimbus