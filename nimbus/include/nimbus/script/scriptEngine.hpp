#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/scene/scene.hpp"

#include <string>
#include <unordered_set>

#if defined(NB_WINDOWS)
#define SCRIPTENGINE_DELEGATE_CALLTYPE __stdcall
#else
#define SCRIPTENGINE_DELEGATE_CALLTYPE
#endif

namespace nimbus
{

struct ScriptEngineInternalData;

class NIMBUS_API ScriptEngine
{
    ////////////////////////////////////////////////////////////////////////////
    // Variables/Types
    ////////////////////////////////////////////////////////////////////////////
   public:
    inline static const wchar_t* k_scriptCoreTypeName = L"Nimbus.ScriptCore, scriptCore";

    //////////////////////////////////////////////////////
    // ScriptInstance class for holding instance info
    //////////////////////////////////////////////////////
    class ScriptInstance : public refCounted
    {
       public:
        inline void onUpdate(float deltaTime)
        {
            s_invokeManagedMethod<void, float>(mp_onUpdateFptr, deltaTime);
        }

        inline void onPhysicsUpdate(float deltaTime)
        {
            s_invokeManagedMethod<void, float>(mp_onPhysicsUpdateFptr, deltaTime);
        }

        ~ScriptInstance()
        {
            s_invokeManagedMethod<void>(mp_onDestroyFptr);
            s_releaseHandle(mp_entityHandle);
        }

       private:
        ScriptInstance(fp_t p_entityHandle,
                       fp_t p_onCreateFptr,
                       fp_t p_onUpdateFptr,
                       fp_t p_onPhysicsUpdateFptr,
                       fp_t p_onDestroyFptr)
            : mp_entityHandle(p_entityHandle),
              mp_onCreateFptr(p_onCreateFptr),
              mp_onUpdateFptr(p_onUpdateFptr),
              mp_onPhysicsUpdateFptr(p_onPhysicsUpdateFptr),
              mp_onDestroyFptr(p_onDestroyFptr)
        {
            s_invokeManagedMethod<void>(mp_onCreateFptr);
        }

        ScriptInstance() = delete;

        fp_t mp_entityHandle        = nullptr;
        fp_t mp_onCreateFptr        = nullptr;
        fp_t mp_onUpdateFptr        = nullptr;
        fp_t mp_onPhysicsUpdateFptr = nullptr;
        fp_t mp_onDestroyFptr       = nullptr;

        friend class ScriptEngine;
    };

   private:
    static ScriptEngineInternalData* sp_data;
    static ref<Scene>                sp_sceneContext;

    ////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////
   public:
    template <typename rT, typename... Args>
    static inline rT s_invokeManagedMethodByName(const std::wstring& name, Args... args)
    {
        typedef rT(SCRIPTENGINE_DELEGATE_CALLTYPE * fn)(Args...);
        return ((fn)s_getStaticMethodPtr(name))(args...);
    }

    template <typename rT, typename... Args>
    static inline rT s_invokeManagedMethod(ip_t* p, Args... args)
    {
        typedef rT(SCRIPTENGINE_DELEGATE_CALLTYPE * fn)(Args...);
        return ((fn)p)(args...);
    }

    static void s_init(const std::string& installPath);

    static void s_destroy();

    static void s_setSceneContext(const ref<Scene>& p_scene);

    inline static ref<Scene> s_getSceneContext()
    {
        return sp_sceneContext;
    }

    static fp_t s_getStaticMethodPtr(const std::wstring& name, const std::wstring& typeName = k_scriptCoreTypeName);

    static void s_freeMemory(ip_t* p);

    static void s_releaseHandle(ip_t* h);

    static bool s_loadScriptAssembly(const std::filesystem::path& assemblyPath);

    static bool s_unloadScriptAssembly();

    static bool s_reloadScriptAssembly(const std::filesystem::path& assemblyPath);

    static std::unordered_set<std::string> s_getScriptAssemblyTypes(const char* p_baseClassFilter = "Nimbus.Entity");

    static ref<ScriptInstance> s_createInstanceOfScriptAssemblyEntity(const std::string& typeName,
                                                                      u32_t              nativeEntityId);
};

}  // namespace nimbus