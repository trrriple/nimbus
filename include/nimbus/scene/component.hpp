#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/renderer/texture.hpp"
#include "nimbus/renderer/font.hpp"
#include "nimbus/scene/camera.hpp"
#include "nimbus/core/window.hpp"

#include "glm.hpp"
#include "gtx/quaternion.hpp"



namespace nimbus
{

class EntityLogic;  // forward decl

struct NameCmp
{
    std::string name;

    NameCmp()               = default;
    NameCmp(const std::string& iname) : name(iname)
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
        initLogic    = []() -> EntityLogic* { return new T(); };
        
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
    TransformCmp()                    = default;
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

    void setTransform(const glm::vec3& itranslation,
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
    ref<Texture> p_texture;
    float        tilingFactor = 1.0f;

    SpriteCmp() = default;
    SpriteCmp(const glm::vec4& icolor) : color(icolor)
    {
    }
    SpriteCmp(const glm::vec4& icolor,
              ref<Texture>&    p_itexture,
              float            itilingFactor = 1.0f)
        : color(icolor), p_texture(p_itexture), tilingFactor(itilingFactor)
    {
    }
};

struct TextCmp
{
    std::string  text;
    Font::Format format;

    TextCmp() = default;
    TextCmp(const std::string& itext, const Font::Format& iformat)
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
    RefCmp(void* p_iref) : p_ref(p_iref)
    {
    }
};

struct WindowRefCmp
{
    Window* p_window;
    WindowRefCmp(Window* p_iwindow) : p_window(p_iwindow)
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