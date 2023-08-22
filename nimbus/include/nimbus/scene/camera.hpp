#pragma once

#include "glm.hpp"

namespace nimbus
{

class Camera : public refCounted
{
   public:
    enum class Type
    {
        orthographic,
        perspective
    };

    enum class Movement
    {
        forward,
        backward,
        left,
        right,
        up,
        down,
    };

    struct Bounds
    {
        glm::vec4 topLeft;
        glm::vec4 topRight;
        glm::vec4 bottomLeft;
        glm::vec4 bottomRight;
    };

    Camera(Type type = Type::orthographic);

    void setType(Type type);

    Type getType() const
    {
        return m_type;
    }

    // processes input received from any position movement input system.
    // Accepts input parameter in the form of camera defined ENUM
    // (to abstract it from windowing systems)
    void processPosiUpdate(Movement direction, f32_t deltaTime);

    // processes input received from a view update.
    void processViewUpdate(const glm::vec2& offset, bool constrainPitch = true);

    void processZoom(f32_t offset);

    glm::mat4& getView();

    glm::mat4& getProjection();

    glm::mat4& getViewProjection();

    Bounds& getVisibleWorldBounds();

    void setAspectRatio(f32_t aspectRatio);

    inline f32_t getAspectRatio() const
    {
        return m_aspectRatio;
    }

    void setNearClip(f32_t near);

    inline f32_t getNearClip()
    {
        return m_type == Type::orthographic ? m_orthoNear : m_near;
    }

    void setFarClip(f32_t far);

    inline f32_t getFarClip()
    {
        return m_type == Type::orthographic ? m_orthoFar : m_far;
    }

    void setPosition(const glm::vec3& position);

    void setZoom(f32_t zoom);

    inline f32_t getZoom()
    {
        return m_zoom;
    }

    void setFov(f32_t fov);

    f32_t getFov()
    {
        return m_fov;
    }

    inline const glm::vec3& getPosition() const
    {
        return m_position;
    }

    void setSpeed(f32_t speed);

    inline f32_t getSpeed() const
    {
        return m_speed;
    }

    void setSensitivity(f32_t sensitivity);

    inline f32_t getSensitivity() const
    {
        return m_sensitivity;
    }

    void setYaw(f32_t yaw);

    inline f32_t getYaw() const
    {
        return m_yaw;
    }

    void setPitch(f32_t pitch);

    inline f32_t getPitch() const
    {
        return m_pitch;
    }

    void setOrthoWidth(f32_t width);

    inline f32_t getOrthoWidth() const
    {
        return m_orthoWidth;
    }

    void setOrthoHeight(f32_t height);

    inline f32_t getOrthoHeight() const
    {
        return m_orthoHeight;
    }

   private:
    glm::mat4 m_projection;
    glm::mat4 m_view;
    glm::mat4 m_viewProjection;
    Bounds    m_worldBounds;

    volatile bool m_staleProjection  = true;
    volatile bool m_staleView        = true;
    volatile bool m_staleWorldBounds = true;

    // location
    glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
    glm::vec3 m_worldUp  = {0.0f, 1.0f, 0.0f};
    glm::vec3 m_front    = {0.0f, 0.0f, -1.0f};
    glm::vec3 m_up;
    glm::vec3 m_right;

    // euler angles
    f32_t m_yaw   = 0.0f;
    f32_t m_pitch = 0.0f;

    // tuning values
    f32_t m_speed       = 10.0f;
    f32_t m_sensitivity = 0.05f;
    f32_t m_fov         = 45.0f;
    f32_t m_zoom        = 1.0f;
    f32_t m_aspectRatio = 1.0;

    f32_t m_near = 0.1f;
    f32_t m_far  = 300.0f;

    f32_t m_orthoNear   = -1.0f;
    f32_t m_orthoFar    = 1.0f;
    f32_t m_orthoWidth  = 10.0f;  // always additionally scaled by aspect ratio
    f32_t m_orthoHeight = 10.0f;

    Type m_type;

    // calculates the front vector from the Camera's (updated) Euler Angles
    void  _updateCameraVectors();
    f32_t _getAspectRatio();
};

}  // namespace nimbus