#pragma once

#include "common.hpp"
#include "glm.hpp"
#include "gtx/quaternion.hpp"

namespace nimbus::util
{

glm::vec2 pixelPosToScreenPos(glm::vec2 pixelPos,
                              float     screenWidth,
                              float     screenHeight);

glm::vec2 pixelSizeToScreenSize(glm::vec2 pixelSize,
                                float     screenWidth,
                                float     screenHeight);

glm::vec2 pixelVelocityToScreenVelocity(glm::vec2 pixelVelocity,
                                        float     screenWidth,
                                        float     screenHeight);


// TODO remove
struct Transform
{
    glm::vec3 translation = {0.0f, 0.0f, 0.0f};
    glm::vec3 rotation    = {0.0f, 0.0f, 0.0f};
    glm::vec3 scale       = {1.0f, 1.0f, 1.0f};

    Transform()                 = default;
    Transform(const Transform&) = default;
    Transform(const glm::vec3& translation) : translation(translation)
    {
    }

    glm::mat4 getModel() const
    {
        glm::mat4 rot = glm::toMat4(glm::quat(rotation));
        
        glm::mat4 preScale = glm::translate(glm::mat4(1.0f), translation) * rot;

        return  glm::scale(preScale, scale);
    }
};

}  // namespace nimbus::util
