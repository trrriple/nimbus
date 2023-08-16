#pragma once

#include "nimbus/core/common.hpp"
#include "glm.hpp"
#include "gtx/quaternion.hpp"

#include <string>
#include <vector>

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

glm::vec2 mapPixToScreen(glm::vec2 pixPos,
                         float     screenMinX,
                         float     screenMaxX,
                         float     screenMinY,
                         float     screenMaxY,
                         int       imgWidth,
                         int       imgHeight);

std::vector<std::string> openFile(const std::string& prompt    = "",
                                  const std::string& startPath = ".",
                                  const std::vector<std::string>& filters
                                  = {"All Files", "*"},
                                  bool multiSelect = false);

std::string saveFile(const std::string&              prompt    = "",
                     const std::string&              startPath = ".",
                     const std::vector<std::string>& filters
                     = {"All Files", "*"});

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

        return glm::scale(preScale, scale);
    }
};

}  // namespace nimbus::util
