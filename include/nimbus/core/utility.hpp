#pragma once

#include "nimbus/core/common.hpp"
#include "glm.hpp"
#include "gtx/quaternion.hpp"
#include "gtx/matrix_decompose.hpp"

#include <string>
#include <vector>

namespace nimbus::util
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Util functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

glm::vec2 pixelPosToScreenPos(glm::vec2 pixelPos, f32_t screenWidth, f32_t screenHeight);

glm::vec2 pixelSizeToScreenSize(glm::vec2 pixelSize, f32_t screenWidth, f32_t screenHeight);

glm::vec2 pixelVelocityToScreenVelocity(glm::vec2 pixelVelocity, f32_t screenWidth, f32_t screenHeight);

glm::vec2 mapPixToScreen(glm::vec2 pixPos,
                         f32_t     screenMinX,
                         f32_t     screenMaxX,
                         f32_t     screenMinY,
                         f32_t     screenMaxY,
                         int       imgWidth,
                         int       imgHeight);

std::vector<std::string> openFile(const std::string&              prompt      = "",
                                  const std::string&              startPath   = ".",
                                  const std::vector<std::string>& filters     = {"All Files", "*"},
                                  bool                            multiSelect = false);

std::string saveFile(const std::string&              prompt    = "",
                     const std::string&              startPath = ".",
                     const std::vector<std::string>& filters   = {"All Files", "*"});

char* readFileAsBytes(const std::string& filepath, u32_t* outSize);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Util classes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Transform
{
   public:
    Transform() = default;
    Transform(const glm::vec3& itranslation);
    Transform(const glm::vec3& itranslation, const glm::vec3& irotation, const glm::vec3& iscale);

    const glm::mat4& getTransform() const;

    void setTransform(const glm::vec3& itranslation, const glm::vec3& irotation, const glm::vec3& iscale);
    void setTransform(const glm::mat4& itransform);

    inline const glm::vec3& getTranslation() const
    {
        return translation;
    }

    inline const glm::vec3& getRotation() const
    {
        return rotation;
    }

    inline const glm::vec3& getScale() const
    {
        return scale;
    }

    void setTranslation(const glm::vec3& itranslation);

    void setTranslationX(f32_t transX);

    void setTranslationY(f32_t transY);

    void setTranslationZ(f32_t transZ);

    void setRotation(const glm::vec3& irotation);

    void setRotationX(f32_t rotX);

    void setRotationY(f32_t rotY);

    void setRotationZ(f32_t rotZ);

    void setScale(const glm::vec3& iscale);

    void setScaleX(f32_t scaleX);

    void setScaleY(f32_t scaleY);

    void setScaleZ(f32_t scaleZ);

    inline bool isScaleLocked() const
    {
        return scaleLocked;
    }

    void setScaleLocked(bool locked);

   private:
    mutable glm::mat4 transform;
    mutable bool      transformStale = true;
    bool              scaleLocked    = false;
    glm::vec3         translation    = {0.0f, 0.0f, 0.0f};
    glm::vec3         rotation       = {0.0f, 0.0f, 0.0f};
    glm::vec3         scale          = {1.0f, 1.0f, 1.0f};
};

}  // namespace nimbus::util
