#pragma once

#include "nimbus/core/common.hpp"
#include "glm.hpp"
#include "gtx/quaternion.hpp"
#include "gtx/matrix_decompose.hpp"

#include <string>
#include <vector>

namespace nimbus::util
{

glm::vec2 pixelPosToScreenPos(glm::vec2 pixelPos, float screenWidth, float screenHeight);

glm::vec2 pixelSizeToScreenSize(glm::vec2 pixelSize, float screenWidth, float screenHeight);

glm::vec2 pixelVelocityToScreenVelocity(glm::vec2 pixelVelocity, float screenWidth, float screenHeight);

glm::vec2 mapPixToScreen(glm::vec2 pixPos,
                         float     screenMinX,
                         float     screenMaxX,
                         float     screenMinY,
                         float     screenMaxY,
                         int       imgWidth,
                         int       imgHeight);

std::vector<std::string> openFile(const std::string&              prompt      = "",
                                  const std::string&              startPath   = ".",
                                  const std::vector<std::string>& filters     = {"All Files", "*"},
                                  bool                            multiSelect = false);

std::string saveFile(const std::string&              prompt    = "",
                     const std::string&              startPath = ".",
                     const std::vector<std::string>& filters   = {"All Files", "*"});

class Transform
{
   public:
    Transform() = default;
    Transform(const glm::vec3& itranslation) : translation(itranslation)
    {
    }
    Transform(const glm::vec3& itranslation, const glm::vec3& irotation, const glm::vec3& iscale)
        : translation(itranslation), rotation(irotation), scale(iscale)
    {
    }

    const glm::mat4& getTransform() const
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

    void setTransform(const glm::vec3& itranslation, const glm::vec3& irotation, const glm::vec3& iscale)
    {
        translation    = itranslation;
        rotation       = irotation;
        scale          = iscale;
        transformStale = true;
    }

    void setTransform(const glm::mat4& itransform)
    {
        glm::quat orientation;
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::decompose(itransform, scale, orientation, translation, skew, perspective);

        rotation = glm::eulerAngles(orientation);

        transform = itransform;

        transformStale = false;
    }

    const glm::vec3& getTranslation() const
    {
        return translation;
    }

    const glm::vec3& getRotation() const
    {
        return rotation;
    }

    const glm::vec3& getScale() const
    {
        return scale;
    }

    void setTranslation(const glm::vec3& itranslation)
    {
        translation    = itranslation;
        transformStale = true;
    }

    void setTranslationX(float transX)
    {
        translation.x  = transX;
        transformStale = true;
    }

    void setTranslationY(float transY)
    {
        translation.y  = transY;
        transformStale = true;
    }

    void setTranslationZ(float transZ)
    {
        translation.z  = transZ;
        transformStale = true;
    }

    void setRotation(const glm::vec3& irotation)
    {
        rotation       = irotation;
        transformStale = true;
    }

    void setRotationX(float rotX)
    {
        rotation.x     = rotX;
        transformStale = true;
    }

    void setRotationY(float rotY)
    {
        rotation.y     = rotY;
        transformStale = true;
    }

    void setRotationZ(float rotZ)
    {
        rotation.z     = rotZ;
        transformStale = true;
    }

    void setScale(const glm::vec3& iscale)
    {
        scale          = iscale;
        transformStale = true;
        scaleLocked    = false;
    }

    void setScaleX(float scaleX)
    {
        if (scaleLocked)
        {
            if (scale.x != 0.0 && scaleX != 0.0)
            {
                float factorYX = abs(scale.y / scale.x);
                float factorZX = abs(scale.z / scale.x);

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

    void setScaleY(float scaleY)
    {
        if (scaleLocked)
        {
            if (scale.y != 0.0 && scaleY != 0.0)
            {
                float factorXY = scale.x / scale.y;
                float factorZY = scale.z / scale.y;

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

    void setScaleZ(float scaleZ)
    {
        if (scaleLocked)
        {
            if (scale.z != 0.0 && scaleZ != 0.0)
            {
                float factorXZ = scale.x / scale.z;
                float factorYZ = scale.y / scale.z;

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

    bool isScaleLocked() const
    {
        return scaleLocked;
    }

    void setScaleLocked(bool locked)
    {
        scaleLocked = locked;
    }

   private:
    mutable glm::mat4 transform;
    mutable bool      transformStale = true;
    bool              scaleLocked    = false;
    glm::vec3         translation    = {0.0f, 0.0f, 0.0f};
    glm::vec3         rotation       = {0.0f, 0.0f, 0.0f};
    glm::vec3         scale          = {1.0f, 1.0f, 1.0f};
};

}  // namespace nimbus::util
