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

namespace nimbus
{

//////////////////////////////////////////////////////
// These won't change for lifetime of program
//////////////////////////////////////////////////////
static Application* gp_appRef    = nullptr;
static Window*      gp_appWinRef = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helpers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void internalCallsInit()
{
    gp_appRef    = &Application::s_get();
    gp_appWinRef = &gp_appRef->getWindow();
}

static inline Entity getEntity(u32_t entityId)
{
    return Entity(entityId, ScriptEngine::s_getSceneContext().raw());
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