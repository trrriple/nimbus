
#include "nimbus/nmpch.hpp"
#include "nimbus/core.hpp"
#include "nimbus/utility.hpp"

#include "glm.hpp"

namespace nimbus::util
{

glm::vec2 pixelPosToScreenPos(glm::vec2 pixelPos,
                              float     screenWidth,
                              float     screenHeight)
{
    glm::vec2 screenPos;
    float     aspectRatio = screenWidth / screenHeight;

    screenPos.x = ((2.0f * pixelPos.x / screenWidth) - 1.0f) * aspectRatio;
    screenPos.y = 1.0f - (2.0f * (pixelPos.y / screenHeight));

    return screenPos;
}

glm::vec2 pixelSizeToScreenSize(glm::vec2 pixelSize,
                                float     screenWidth,
                                float     screenHeight)
{
    glm::vec2 screenSize;
    float     aspectRatio = screenWidth / screenHeight;

    screenSize.x = 2.0f * (pixelSize.x / screenWidth) * aspectRatio;
    screenSize.y = 2.0f * (pixelSize.y / screenHeight);

    return screenSize;
}

glm::vec2 pixelVelocityToScreenVelocity(glm::vec2 pixelVelocity,
                                        float     screenWidth,
                                        float     screenHeight)
{
    glm::vec2 screenVelocity;
    float     aspectRatio = screenWidth / screenHeight;

    screenVelocity.x = 2.0f * (pixelVelocity.x / screenWidth) * aspectRatio;
    screenVelocity.y = -2.0f * (pixelVelocity.y / screenHeight);

    return screenVelocity;
}

}  // namespace nimbus::util
