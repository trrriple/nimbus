#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/renderer/texture.hpp"
#include "nimbus/renderer/font.hpp"
#include "nimbus/scene/camera.hpp"
#include "nimbus/core/window.hpp"
#include "nimbus/core/guid.hpp"

#include "glm.hpp"
#include "gtx/quaternion.hpp"
#include "gtx/matrix_decompose.hpp"

namespace nimbus
{

class EntityLogic;  // forward decl

struct GuidCmp
{
    Guid     guid;
    uint32_t creationOrder;
    GuidCmp(uint32_t icreationOrder) : creationOrder(icreationOrder)
    {

    }

    GuidCmp() = delete;
};

struct NameCmp
{
    std::string name;

    NameCmp() = default;
    NameCmp(const std::string& iname) noexcept : name(iname)
    {
    }
};

struct NativeLogicCmp
{
    EntityLogic* p_logic = nullptr;

    EntityLogic* (*initLogic)();
    void (*destroyLogic)(NativeLogicCmp*);

    template <typename T>
    void bind()
    {
        initLogic = []() -> EntityLogic* { return new T(); };

        // TOOD figure out why this can't get called
        destroyLogic = [](NativeLogicCmp* nsc)
        {
            delete nsc->p_logic;
            nsc->p_logic = nullptr;
        };
    }
};

struct TransformCmp
{
    TransformCmp() = default;
    TransformCmp(const glm::vec3& itranslation) noexcept
        : translation(itranslation)
    {
    }
    TransformCmp(const glm::vec3& itranslation,
                 const glm::vec3& irotation,
                 const glm::vec3& iscale) noexcept
        : translation(itranslation), rotation(irotation), scale(iscale)
    {
    }

    const glm::mat4& getTransform() const noexcept
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

    void setTransform(const glm::vec3& itranslation,
                      const glm::vec3& irotation,
                      const glm::vec3& iscale) noexcept
    {
        translation    = itranslation;
        rotation       = irotation;
        scale          = iscale;
        transformStale = true;
    }

    void setTransform(const glm::mat4& itransform) noexcept
    {
        glm::quat orientation;
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::decompose(
            itransform, scale, orientation, translation, skew, perspective);

        rotation = glm::eulerAngles(orientation);

        transform = itransform;
    }

    const glm::vec3& getTranslation() const noexcept
    {
        return translation;
    }

    const glm::vec3& getRotation() const noexcept
    {
        return rotation;
    }

    const glm::vec3& getScale() const noexcept
    {
        return scale;
    }

    void setTranslation(const glm::vec3& itranslation) noexcept
    {
        translation    = itranslation;
        transformStale = true;
    }

    void setTranslationX(float transX) noexcept
    {
        translation.x  = transX;
        transformStale = true;
    }

    void setTranslationY(float transY) noexcept
    {
        translation.y  = transY;
        transformStale = true;
    }

    void setTranslationZ(float transZ) noexcept
    {
        translation.z  = transZ;
        transformStale = true;
    }

    void setRotation(const glm::vec3& irotation) noexcept
    {
        rotation       = irotation;
        transformStale = true;
    }

    void setRotationX(float rotX) noexcept
    {
        rotation.x     = rotX;
        transformStale = true;
    }

    void setRotationY(float rotY) noexcept
    {
        rotation.y     = rotY;
        transformStale = true;
    }

    void setRotationZ(float rotZ) noexcept
    {
        rotation.z     = rotZ;
        transformStale = true;
    }

    void setScale(const glm::vec3& iscale) noexcept
    {
        scale          = iscale;
        transformStale = true;
        scaleLocked    = false;
    }

    void setScaleX(float scaleX) noexcept
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

    void setScaleY(float scaleY) noexcept
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

    void setScaleZ(float scaleZ) noexcept
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

    bool isScaleLocked() const noexcept
    {
        return scaleLocked;
    }

    void setScaleLocked(bool locked) noexcept
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

struct SpriteCmp
{
    glm::vec4    color{1.0f};
    ref<Texture> p_texture;
    float        tilingFactor = 1.0f;

    SpriteCmp() = default;
    SpriteCmp(const glm::vec4& icolor) noexcept : color(icolor)
    {
    }
    SpriteCmp(const glm::vec4& icolor,
              ref<Texture>&    p_itexture,
              float            itilingFactor = 1.0f) noexcept
        : color(icolor), p_texture(p_itexture), tilingFactor(itilingFactor)
    {
    }
};

struct TextCmp
{
    std::string  text;
    Font::Format format;

    TextCmp() = default;
    TextCmp(const std::string& itext, const Font::Format& iformat) noexcept
        : text(itext), format(iformat)
    {
    }
};

struct CameraCmp
{
    Camera camera;
    bool   primary     = true;
    bool   fixedAspect = true;

    CameraCmp() = default;
};

struct RefCmp
{
    void* p_ref;
    RefCmp(void* p_iref) noexcept : p_ref(p_iref)
    {
    }
};

struct WindowRefCmp
{
    Window* p_window;
    WindowRefCmp(Window* p_iwindow) noexcept : p_window(p_iwindow)
    {
    }
};

template <typename... Component>
struct ComponentGroup
{
};

using AllComponents = ComponentGroup<NativeLogicCmp,
                                     NameCmp,
                                     TransformCmp,
                                     SpriteCmp,
                                     TextCmp,
                                     CameraCmp,
                                     RefCmp,
                                     WindowRefCmp>;

}  // namespace nimbus