
#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"
#include "nimbus/core/utility.hpp"

#include "glm.hpp"

#include "portable-file-dialogs.h"

#include <fstream>

#include "nimbus/platform/os/os.h"


namespace nimbus::util
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Util functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec2 pixelPosToScreenPos(glm::vec2 pixelPos, f32_t screenWidth, f32_t screenHeight)
{
    glm::vec2 screenPos;
    f32_t     aspectRatio = screenWidth / screenHeight;

    screenPos.x = ((2.0f * pixelPos.x / screenWidth) - 1.0f) * aspectRatio;
    screenPos.y = 1.0f - (2.0f * (pixelPos.y / screenHeight));

    return screenPos;
}

glm::vec2 pixelSizeToScreenSize(glm::vec2 pixelSize, f32_t screenWidth, f32_t screenHeight)
{
    glm::vec2 screenSize;
    f32_t     aspectRatio = screenWidth / screenHeight;

    screenSize.x = 2.0f * (pixelSize.x / screenWidth) * aspectRatio;
    screenSize.y = 2.0f * (pixelSize.y / screenHeight);

    return screenSize;
}

glm::vec2 pixelVelocityToScreenVelocity(glm::vec2 pixelVelocity, f32_t screenWidth, f32_t screenHeight)
{
    glm::vec2 screenVelocity;
    f32_t     aspectRatio = screenWidth / screenHeight;

    screenVelocity.x = 2.0f * (pixelVelocity.x / screenWidth) * aspectRatio;
    screenVelocity.y = -2.0f * (pixelVelocity.y / screenHeight);

    return screenVelocity;
}

glm::vec2 mapPixToScreen(glm::vec2 pixPos,
                         f32_t     screenMinX,
                         f32_t     screenMaxX,
                         f32_t     screenMinY,
                         f32_t     screenMaxY,
                         int       imgWidth,
                         int       imgHeight)
{
    // Normalize pixel values to the range [0, 1]
    f32_t normX = pixPos.x / imgWidth;
    f32_t normY = pixPos.y / imgHeight;

    // Map normalized pixel values to screen space
    glm::vec2 screen;
    screen.x = screenMinX + normX * (screenMaxX - screenMinX);
    screen.y = screenMinY + normY * (screenMaxY - screenMinY);

    return screen;
}

std::vector<std::string> openFileDialog(const std::string&              prompt,
                                        const std::string&              startPath,
                                        const std::vector<std::string>& filters,
                                        bool                            multiSelect)
{
    return pfd::open_file(prompt, startPath, filters, multiSelect ? pfd::opt::multiselect : pfd::opt::none).result();
}

std::string saveFileDialog(const std::string&              prompt,
                           const std::string&              startPath,
                           const std::vector<std::string>& filters)
{
    return pfd::save_file(prompt, startPath, filters, pfd::opt::none).result();
}

char* readFileAsBytes(const std::string& filepath, u32_t* outSize)
{
    std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

    if (!stream)
    {
        // Failed to open the file
        return nullptr;
    }

    std::streampos end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    u32_t size = end - stream.tellg();

    if (size == 0)
    {
        // File is empty
        return nullptr;
    }

    char* buffer = new char[size];
    stream.read((char*)buffer, size);
    stream.close();

    *outSize = size;
    return buffer;
}

std::string getExecutablePath()
{
#if defined(NB_WINDOWS)
    char buffer[MAX_PATH];
    GetModuleFileName(nullptr, buffer, MAX_PATH);
    return std::filesystem::path(buffer).generic_string();
#elif defined(NB_LINUX)
    char    buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1)
    {
        buffer[len] = '\0';
        return std::filesystem::path(buffer).generic_string();
    }
#endif
    return "";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Util classes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Transform::Transform(const glm::vec3& itranslation) : translation(itranslation)
{
}
Transform::Transform(const glm::vec3& itranslation, const glm::vec3& irotation, const glm::vec3& iscale)
    : translation(itranslation), rotation(irotation), scale(iscale)
{
}

const glm::mat4& Transform::getTransform() const
{
    if (transformStale)
    {
        glm::mat4 rot = glm::toMat4(glm::quat(rotation));

        transform = glm::translate(glm::mat4(1.0f), translation) * rot;

        transform = glm::scale(transform, scale);

        transformStale = false;
    }

    return transform;
}

void Transform::setTransform(const glm::vec3& itranslation, const glm::vec3& irotation, const glm::vec3& iscale)
{
    translation    = itranslation;
    rotation       = irotation;
    scale          = iscale;
    transformStale = true;
}

void Transform::setTransform(const glm::mat4& itransform)
{
    glm::quat orientation;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(itransform, scale, orientation, translation, skew, perspective);

    rotation = glm::eulerAngles(orientation);

    transform = itransform;

    transformStale = false;
}

void Transform::setTranslation(const glm::vec3& itranslation)
{
    translation    = itranslation;
    transformStale = true;
}

void Transform::setTranslationX(f32_t transX)
{
    translation.x  = transX;
    transformStale = true;
}

void Transform::setTranslationY(f32_t transY)
{
    translation.y  = transY;
    transformStale = true;
}

void Transform::setTranslationZ(f32_t transZ)
{
    translation.z  = transZ;
    transformStale = true;
}

void Transform::setRotation(const glm::vec3& irotation)
{
    rotation       = irotation;
    transformStale = true;
}

void Transform::setRotationX(f32_t rotX)
{
    rotation.x     = rotX;
    transformStale = true;
}

void Transform::setRotationY(f32_t rotY)
{
    rotation.y     = rotY;
    transformStale = true;
}

void Transform::setRotationZ(f32_t rotZ)
{
    rotation.z     = rotZ;
    transformStale = true;
}

void Transform::setScale(const glm::vec3& iscale)
{
    scale          = iscale;
    transformStale = true;
    scaleLocked    = false;
}

void Transform::setScaleX(f32_t scaleX)
{
    if (scaleLocked)
    {
        if (scale.x != 0.0 && scaleX != 0.0)
        {
            f32_t factorYX = abs(scale.y / scale.x);
            f32_t factorZX = abs(scale.z / scale.x);

            scale.x = scaleX;
            scale.y = scaleX * factorYX;
            scale.z = scaleX * factorZX;
        }
    }
    else
    {
        scale.x = scaleX;
    }

    transformStale = true;
}

void Transform::setScaleY(f32_t scaleY)
{
    if (scaleLocked)
    {
        if (scale.y != 0.0 && scaleY != 0.0)
        {
            f32_t factorXY = scale.x / scale.y;
            f32_t factorZY = scale.z / scale.y;

            scale.y = scaleY;
            scale.x = scaleY * factorXY;
            scale.z = scaleY * factorZY;
        }
    }
    else
    {
        scale.y = scaleY;
    }

    transformStale = true;
}

void Transform::setScaleZ(f32_t scaleZ)
{
    if (scaleLocked)
    {
        if (scale.z != 0.0 && scaleZ != 0.0)
        {
            f32_t factorXZ = scale.x / scale.z;
            f32_t factorYZ = scale.y / scale.z;

            scale.z = scaleZ;
            scale.x = scaleZ * factorXZ;
            scale.y = scaleZ * factorYZ;
        }
    }
    else
    {
        scale.z = scaleZ;
    }

    transformStale = true;
}

void Transform::setScaleLocked(bool locked)
{
    scaleLocked = locked;
}

}  // namespace nimbus::util
