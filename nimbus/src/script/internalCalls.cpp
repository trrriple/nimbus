#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/script/internalCalls.hpp"
#include "nimbus/core/application.hpp"

#include "nimbus/scene/entity.hpp"
#include "nimbus/script/scriptEngine.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#define _CRT_SECURE_NO_WARNINGS
#include "gtx/string_cast.hpp"
#undef _CRT_SECURE_NO_WARNINGS
#include "nimbus.hpp"

#include <unordered_map>

namespace nimbus
{

//////////////////////////////////////////////////////
// These won't change for lifetime of program
//////////////////////////////////////////////////////
static Application*                                           gp_appRef    = nullptr;
static Window*                                                gp_appWinRef = nullptr;
static std::unordered_map<ip_t, std::function<bool(Entity&)>> s_hasComponentFuncs;
static std::unordered_map<ip_t, std::function<void(Entity&)>> s_createComponentFuncs;
static std::unordered_map<ip_t, std::function<void(Entity&)>> s_removeComponentFuncs;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helpers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline Entity getEntity(u32_t entityId)
{
    return Entity(entityId, ScriptEngine::s_getSceneContext().raw());
}

std::string convertToCSharpNamespace(const std::string& cppNamespace)
{
    std::string        result;
    std::istringstream iss(cppNamespace);
    std::string        word;

    while (std::getline(iss, word, ':'))
    {
        // Skip single colons (since C++ uses :: as a separator)
        if (word.empty())
            continue;

        if (!word.empty())
        {
            word[0] = std::toupper(word[0]);
        }

        result += word;
        if (!iss.eof())
        {
            result += '.';
        }
    }

    // Remove trailing dot, if present
    if (!result.empty() && result.back() == '.')
    {
        result.pop_back();
    }

    return result;
}

template <typename T>
void registerComponentType()
{
    std::string typeNameWithNameSpace(util::getTypeName<T>());

    std::string typeNameCSharp = convertToCSharpNamespace(typeNameWithNameSpace);

    ip_t typeHandle
        = ScriptEngine::s_invokeManagedMethodByName<ip_t, const char*>(L"GetTypeHandle", typeNameCSharp.c_str());

    NB_CORE_ASSERT_STATIC(typeHandle != 0, "Didn't find %s in scriptCore Assembly!", typeNameCSharp.c_str());

    s_hasComponentFuncs[typeHandle]    = [](Entity& entity) { return entity.hasComponent<T>(); };
    s_createComponentFuncs[typeHandle] = [](Entity& entity) { entity.addComponent<T>(); };
    s_removeComponentFuncs[typeHandle] = [](Entity& entity) { entity.removeComponent<T>(); };
}

void internalCallsInit()
{
    gp_appRef    = &Application::s_get();
    gp_appWinRef = &gp_appRef->getWindow();

    registerComponentType<GuidCmp>();
    registerComponentType<NameCmp>();
    registerComponentType<AncestryCmp>();
    registerComponentType<ScriptCmp>();
    registerComponentType<NativeLogicCmp>();
    registerComponentType<TransformCmp>();
    registerComponentType<SpriteCmp>();
    registerComponentType<TextCmp>();
    registerComponentType<ParticleEmitterCmp>();
    registerComponentType<RigidBody2DCmp>();
    registerComponentType<CameraCmp>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Components
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INTERNAL_CALL bool ic_hasComponent(u32_t entityId)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Logging
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INTERNAL_CALL void ic_coreInfo(char* msg)
{
    Log::scriptCoreInfo(msg);
}

INTERNAL_CALL void ic_coreWarn(char* msg)
{
    Log::scriptCoreWarn(msg);
}

INTERNAL_CALL void ic_coreError(char* msg)
{
    Log::scriptCoreError(msg);
}

INTERNAL_CALL void ic_coreCritical(char* msg)
{
    Log::scriptCoreCritical(msg);
}

INTERNAL_CALL void ic_coreTrace(char* msg)
{
    Log::scriptCoreTrace(msg);
}

INTERNAL_CALL void ic_info(char* msg)
{
    Log::scriptInfo(msg);
}

INTERNAL_CALL void ic_warn(char* msg)
{
    Log::scriptWarn(msg);
}

INTERNAL_CALL void ic_error(char* msg)
{
    Log::scriptError(msg);
}

INTERNAL_CALL void ic_critical(char* msg)
{
    Log::scriptCritical(msg);
}

INTERNAL_CALL void ic_trace(char* msg)
{
    Log::scriptTrace(msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Transform
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// World Transform
//////////////////////////////////////////////////////
///////////////////////////
// Getters
///////////////////////////
INTERNAL_CALL glm::mat4 ic_getWorldTransform(u32_t entityId)
{
    auto& tc = getEntity(entityId).getComponent<TransformCmp>();

    return tc.world.getTransform();
}

INTERNAL_CALL glm::vec3 ic_getWorldTranslation(u32_t entityId)
{
    auto& tc = getEntity(entityId).getComponent<TransformCmp>();

    return tc.world.getTranslation();
}

INTERNAL_CALL glm::vec3 ic_getWorldRotation(u32_t entityId)
{
    auto& tc = getEntity(entityId).getComponent<TransformCmp>();

    return tc.world.getRotation();
}

INTERNAL_CALL glm::vec3 ic_getWorldScale(u32_t entityId)
{
    auto& tc = getEntity(entityId).getComponent<TransformCmp>();

    return tc.world.getScale();
}

// no point to set world transform from script, it's calculated from local

//////////////////////////////////////////////////////
// Local Transform
//////////////////////////////////////////////////////
///////////////////////////
// Getters
///////////////////////////
INTERNAL_CALL glm::mat4 ic_getLocalTransform(u32_t entityId)
{
    auto& tc = getEntity(entityId).getComponent<TransformCmp>();

    return tc.local.getTransform();
}

INTERNAL_CALL glm::vec3 ic_getLocalTranslation(u32_t entityId)
{
    auto& tc = getEntity(entityId).getComponent<TransformCmp>();

    return tc.local.getTranslation();
}

INTERNAL_CALL glm::vec3 ic_getLocalRotation(u32_t entityId)
{
    auto& tc = getEntity(entityId).getComponent<TransformCmp>();

    return tc.local.getRotation();
}

INTERNAL_CALL glm::vec3 ic_getLocalScale(u32_t entityId)
{
    auto& tc = getEntity(entityId).getComponent<TransformCmp>();

    return tc.local.getScale();
}

///////////////////////////
// Setters
///////////////////////////
INTERNAL_CALL void ic_setLocalTransform(u32_t entityId, glm::mat4* p_transform)
{
    auto& tc = getEntity(entityId).getComponent<TransformCmp>();

    tc.local.setTransform(*p_transform);
}

INTERNAL_CALL void ic_setLocalTranslation(u32_t entityId, glm::vec3* p_translation)
{
    auto& tc = getEntity(entityId).getComponent<TransformCmp>();

    tc.local.setTranslation(*p_translation);
}

INTERNAL_CALL void ic_setLocalRotation(u32_t entityId, glm::vec3* p_rotation)
{
    auto& tc = getEntity(entityId).getComponent<TransformCmp>();

    tc.local.setRotation(*p_rotation);
}

INTERNAL_CALL void ic_setLocalScale(u32_t entityId, glm::vec3* p_scale)
{
    auto& tc = getEntity(entityId).getComponent<TransformCmp>();

    tc.local.setScale(*p_scale);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Input
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INTERNAL_CALL bool ic_keyPresed(ScanCode scanCode)
{
    return gp_appWinRef->keyDown(scanCode);
}

INTERNAL_CALL bool ic_modKeyPressed(KeyMod keyMod)
{
    return gp_appWinRef->modKeyDown(keyMod);
}

INTERNAL_CALL bool ic_mouseButtonPressed(MouseButton mouseButton)
{
    return gp_appWinRef->nouseButtonDown(mouseButton);
}

}  // namespace nimbus