#pragma once
#include "nimbus/core/common.hpp"

#include <string>

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
    // Variables
    ////////////////////////////////////////////////////////////////////////////
   public:
    inline static const wchar_t* k_scriptCoreTypeName = L"Nimbus.ScriptCore, scriptCore";

   private:
    static ScriptEngineInternalData* s_data;

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
    static inline rT s_invokeManagedMethod(void* p, Args... args)
    {
        typedef rT(SCRIPTENGINE_DELEGATE_CALLTYPE * fn)(Args...);
        return ((fn)p)(args...);
    }

    static void  s_init(const std::string& installPath);
    static void  s_destroy();
    static void* s_getStaticMethodPtr(const std::wstring& name, const std::wstring& typeName = k_scriptCoreTypeName);
    static void  s_freeMemory(void* p);
    static void  s_loadScriptAssembly();
    static void  s_unloadScriptAssembly();
    static void  s_reloadScriptAssembly();

    static void s_testCallScript();
    static void s_getScriptAssemblyTypes(const char* p_baseClassFilter = "Nimbus.Entity");
};

}  // namespace nimbus