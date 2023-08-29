#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/renderer/texture.hpp"
#include "nimbus/renderer/font.hpp"
#include "nimbus/scene/camera.hpp"
#include "nimbus/core/window.hpp"
#include "nimbus/core/guid.hpp"
#include "nimbus/renderer/particleEmitter.hpp"
#include "nimbus/scene/entity.hpp"
#include "nimbus/core/utility.hpp"
#include "nimbus/physics/physics2D.hpp"
#include "nimbus/script/scriptEngine.hpp"

#include "glm.hpp"

namespace nimbus
{

class EntityLogic;  // forward decl

struct GuidCmp
{
    Guid  guid;
    u32_t sequenceIndex;
    GuidCmp(u32_t icreationOrder) : sequenceIndex(icreationOrder)
    {
    }

    ////////////////////////////////////////////////////////////////////////////
    // For use by Scene::_addEntity only
    ////////////////////////////////////////////////////////////////////////////
    GuidCmp(u32_t icreationOrder, const std::string& guidStr) : sequenceIndex(icreationOrder)
    {
        guid = Guid(guidStr);
    }

    GuidCmp() = default;
};

struct NameCmp
{
    std::string name;

    NameCmp() = default;
    NameCmp(const std::string& iname) : name(iname)
    {
    }
};

struct AncestryCmp
{
    Entity              parent;
    std::vector<Entity> children;

    AncestryCmp() = default;
    AncestryCmp(Entity iparent) : parent(iparent)
    {
    }
};

struct ScriptCmp
{
    std::string                       scriptEntityName;
    ref<ScriptEngine::ScriptInstance> p_scriptInstance;

    ScriptCmp() = default;
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
    util::Transform local;
    util::Transform world;

    TransformCmp() = default;

    TransformCmp(const glm::vec3& itranslation)
    {
        local.setTranslation(itranslation);
    }
    TransformCmp(const glm::vec3& itranslation, const glm::vec3& irotation, const glm::vec3& iscale)
    {
        local.setTranslation(itranslation);
        local.setRotation(irotation);
        local.setScale(iscale);
    }
};

struct SpriteCmp
{
    glm::vec4    color{1.0f};
    ref<Texture> p_texture;
    f32_t        tilingFactor = 1.0f;

    SpriteCmp() = default;
    SpriteCmp(const glm::vec4& icolor) : color(icolor)
    {
    }
    SpriteCmp(const glm::vec4& icolor, ref<Texture> p_itexture, f32_t itilingFactor = 1.0f)
        : color(icolor), p_texture(p_itexture), tilingFactor(itilingFactor)
    {
    }
};

struct TextCmp
{
    std::string  text;
    Font::Format format;

    TextCmp() = default;
    TextCmp(const std::string& itext, const Font::Format& iformat) : text(itext), format(iformat)
    {
    }
};

struct ParticleEmitterCmp
{
    u32_t                       numParticles = 100;
    ParticleEmitter::Parameters parameters;
    ref<Texture>                p_texture = nullptr;
    ref<ParticleEmitter>        p_emitter = nullptr;

    ParticleEmitterCmp()
    {
        parameters.colors.push_back({glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)});
    }
    ParticleEmitterCmp(u32_t                              inumParticles,
                       const ParticleEmitter::Parameters& iparameters,
                       ref<Texture>                       p_itexture = nullptr)
        : numParticles(inumParticles), parameters(iparameters), p_texture(p_itexture)
    {
    }
};

struct RigidBody2DCmp
{
    Physics2D::RigidBodySpec spec;
    Physics2D::FixtureSpec   fixSpec;

    // fixSpec will point to one of these if fixture (collider) is desired
    Physics2D::Rectangle rectShape;
    Physics2D::Circle    circShape;

    ref<Physics2D::RigidBody> p_body;

    // TODO: temporary hack until scene duplication is done
    util::Transform preSimTransform;

    RigidBody2DCmp() = default;
    RigidBody2DCmp(const Physics2D::RigidBodySpec& ispec, const Physics2D::FixtureSpec& ifixSpec)
        : spec(ispec), fixSpec(ifixSpec)
    {
    }
};

struct CameraCmp
{
    Camera camera;
    bool   primary     = true;
    bool   fixedAspect = false;

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

using AllComponents = ComponentGroup<GuidCmp,
                                     NameCmp,
                                     AncestryCmp,
                                     ScriptCmp,
                                     NativeLogicCmp,
                                     TransformCmp,
                                     SpriteCmp,
                                     TextCmp,
                                     CameraCmp,
                                     RefCmp,
                                     WindowRefCmp>;

}  // namespace nimbus