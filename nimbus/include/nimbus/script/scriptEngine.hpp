#pragma once
#include "nimbus/core/common.hpp"

#include <string>

namespace nimbus
{

struct ScriptEngineInternalData;

class NIMBUS_API ScriptEngine
{
    ////////////////////////////////////////////////////////////////////////////
    // Variables
    ////////////////////////////////////////////////////////////////////////////
   public:
   private:
    static ScriptEngineInternalData* s_data;

    ////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////
   public:
    static void  s_init(const std::string& installPath);
    static void  s_destroy();
    static void* s_getStaticMethodPtr(const std::wstring& name);
    static void  s_freeMemory(void* p);
    static void  s_loadScriptAssembly();
    static void  s_unloadScriptAssembly();
    static void  s_reloadScriptAssembly();

    static void s_testCallScript();
    static void s_reflectOnScriptAssembly();

};

}  // namespace nimbus