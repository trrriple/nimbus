
#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"
#include "nimbus/core/utility.hpp"

#include "glm.hpp"

#include "portable-file-dialogs.h"

namespace nimbus::util
{

glm::vec2 pixelPosToScreenPos(glm::vec2 pixelPos,
                              float     screenWidth,
                              float     screenHeight) noexcept
{
    glm::vec2 screenPos;
    float     aspectRatio = screenWidth / screenHeight;

    screenPos.x = ((2.0f * pixelPos.x / screenWidth) - 1.0f) * aspectRatio;
    screenPos.y = 1.0f - (2.0f * (pixelPos.y / screenHeight));

    return screenPos;
}

glm::vec2 pixelSizeToScreenSize(glm::vec2 pixelSize,
                                float     screenWidth,
                                float     screenHeight) noexcept
{
    glm::vec2 screenSize;
    float     aspectRatio = screenWidth / screenHeight;

    screenSize.x = 2.0f * (pixelSize.x / screenWidth) * aspectRatio;
    screenSize.y = 2.0f * (pixelSize.y / screenHeight);

    return screenSize;
}

glm::vec2 pixelVelocityToScreenVelocity(glm::vec2 pixelVelocity,
                                        float     screenWidth,
                                        float     screenHeight) noexcept
{
    glm::vec2 screenVelocity;
    float     aspectRatio = screenWidth / screenHeight;

    screenVelocity.x = 2.0f * (pixelVelocity.x / screenWidth) * aspectRatio;
    screenVelocity.y = -2.0f * (pixelVelocity.y / screenHeight);

    return screenVelocity;
}

glm::vec2 mapPixToScreen(glm::vec2 pixPos,
                         float     screenMinX,
                         float     screenMaxX,
                         float     screenMinY,
                         float     screenMaxY,
                         int       imgWidth,
                         int       imgHeight) noexcept
{
    // Normalize pixel values to the range [0, 1]
    float normX = pixPos.x / imgWidth;
    float normY = pixPos.y / imgHeight;

    // Map normalized pixel values to screen space
    glm::vec2 screen;
    screen.x = screenMinX + normX * (screenMaxX - screenMinX);
    screen.y = screenMinY + normY * (screenMaxY - screenMinY);

    return screen;
}

std::vector<std::string> openFile(const std::string&              prompt,
                                  const std::string&              startPath,
                                  const std::vector<std::string>& filters,
                                  bool multiSelect) noexcept
{
    return pfd::open_file(prompt,
                          startPath,
                          filters,
                          multiSelect ? pfd::opt::multiselect : pfd::opt::none)
        .result();
}

std::string saveFile(const std::string&              prompt,
                     const std::string&              startPath,
                     const std::vector<std::string>& filters) noexcept
{
    return pfd::save_file(prompt, startPath, filters, pfd::opt::none).result();
}

}  // namespace nimbus::util
