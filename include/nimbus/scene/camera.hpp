#pragma once

#include "glm.hpp"

namespace nimbus
{

class Camera : public refCounted
{
   public:
    enum class Type
    {
        ORTHOGRAPHIC,
        PERSPECTIVE
    };

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

    Camera(Type type = Type::ORTHOGRAPHIC);

    void setType(Type type);

    Type getType() const
    {
        return m_type;
    }

    // processes input received from any position movement input system.
    // Accepts input parameter in the form of camera defined ENUM
    // (to abstract it from windowing systems)
    void processPosiUpdate(Movement direction, float deltaTime);

    // processes input received from a view update.
    void processViewUpdate(const glm::vec2& offset, bool constrainPitch = true);

    void processZoom(float offset);

    glm::mat4& getView();

    glm::mat4& getProjection();

    glm::mat4& getViewProjection();

    Bounds& getVisibleWorldBounds();

    void setAspectRatio(float aspectRatio);

    inline float getAspectRatio() const
    {
        return m_aspectRatio;
    }

    void setNearClip(float near);

    inline float getNearClip()
    {
        return m_type == Type::ORTHOGRAPHIC ? m_orthoNear : m_near;
    }

    void setFarClip(float far);

    inline float getFarClip()
    {
        return m_type == Type::ORTHOGRAPHIC ? m_orthoFar : m_far;
    }

    void setPosition(const glm::vec3& position);

    void setZoom(float zoom);

    inline float getZoom()
    {
        return m_zoom;
    }

    void setFov(float fov);

    float getFov()
    {
        return m_fov;
    }

    inline const glm::vec3& getPosition() const
    {
        return m_position;
    }

    void setSpeed(float speed);

    inline float getSpeed() const
    {
        return m_speed;
    }

    void setSensitivity(float sensitivity);

    inline float getSensitivity() const
    {
        return m_sensitivity;
    }

    void setYaw(float yaw);

    inline float getYaw() const
    {
        return m_yaw;
    }

    void setPitch(float pitch);

    inline float getPitch() const
    {
        return m_pitch;
    }

    void setOrthoWidth(float width);

    inline float getOrthoWidth() const
    {
        return m_orthoWidth;
    }

    void setOrthoHeight(float height);

    inline float getOrthoHeight() const
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
    float m_yaw   = 0.0f;
    float m_pitch = 0.0f;

    // tuning values
    float m_speed       = 10.0f;
    float m_sensitivity = 0.05f;
    float m_fov         = 45.0f;
    float m_zoom        = 1.0f;
    float m_aspectRatio = 1.0;

    float m_near = 0.1f;
    float m_far  = 300.0f;

    float m_orthoNear   = -1.0f;
    float m_orthoFar    = 1.0f;
    float m_orthoWidth  = 10.0f;  // always additionally scaled by aspect ratio
    float m_orthoHeight = 10.0f;

    Type m_type;

    // calculates the front vector from the Camera's (updated) Euler Angles
    void  _updateCameraVectors();
    float _getAspectRatio();
};

}  // namespace nimbus