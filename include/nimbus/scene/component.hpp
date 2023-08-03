#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/renderer/texture.hpp"
#include "nimbus/renderer/camera.hpp"

#include "glm.hpp"
#include "gtx/quaternion.hpp"



namespace nimbus
{

struct TransformCmp
{
    TransformCmp()                    = default;
    // TransformCmp(const TransformCmp&) = default;
    TransformCmp(const glm::vec3& itranslation) : translation(itranslation)
    {
    }
    TransformCmp(const glm::vec3& itranslation,
                 const glm::vec3& irotation,
                 const glm::vec3& iscale)
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

    void setPosition(const glm::vec3& itranslation,
                     const glm::vec3& irotation,
                     const glm::vec3& iscale)
    {
        translation    = itranslation;
        rotation       = irotation;
        scale          = iscale;
        transformStale = true;
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

    void getRotation(const glm::vec3& irotation)
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
    }
    void setScaleX(float scaleX)
    {
        scale.x        = scaleX;
        transformStale = true;
    }

    void setScaleY(float scaleY)
    {
        scale.y        = scaleY;
        transformStale = true;
    }

    void setScaleZ(float scaleZ)
    {
        scale.z        = scaleZ;
        transformStale = true;
    }

   private:
    mutable glm::mat4 transform;
    mutable bool      transformStale = true;
    glm::vec3         translation    = {0.0f, 0.0f, 0.0f};
    glm::vec3         rotation       = {0.0f, 0.0f, 0.0f};
    glm::vec3         scale          = {1.0f, 1.0f, 1.0f};
};

struct SpriteCmp
{
    glm::vec4    color{1.0f};
    ref<Texture> texture;
    float        tilingFactor = 1.0f;

    SpriteCmp()                 = default;
    // SpriteCmp(const SpriteCmp&) = default;
    SpriteCmp(const glm::vec4& icolor) : color(icolor)
    {
    }
    SpriteCmp(const glm::vec4& icolor,
              ref<Texture>&    itexture,
              float            itilingFactor = 1.0f)
        : color(icolor), texture(itexture), tilingFactor(itilingFactor)
    {
    }
};


struct CameraCmp
{
    Camera camera;

    CameraCmp()                            = default;
    // CameraCmp(const CameraCmp&)            = default;
    // CameraCmp& operator=(const CameraCmp&) = default;
};


}  // namespace nimbus