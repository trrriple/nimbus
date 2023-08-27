#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/script/scriptEngine.hpp"
#include "nimbus/core/utility.hpp"


#include "nimbus/platform/os/os.h"
#include "nimbus/platform/os/headers.h"
#include "nimbus/script/internalCalls.hpp"

#include "nethost.h"
#include "coreclr_delegates.h"
#include "hostfxr.h"

#include <filesystem>

#if defined(NB_WINDOWS)
#ifdef _WCHAR_T_DEFINED
typedef wchar_t char_t;
#else
typedef unsigned short char_t;
#endif
#else
typedef char char_t;
#endif

namespace nimbus
{

struct ScriptEngineInternalData
{
    //////////////////////////////////////////////////////
    // HostFxr Pointers
    //////////////////////////////////////////////////////
    hostfxr_initialize_for_runtime_config_fn      mp_initRuntimeFptr = nullptr;
    hostfxr_initialize_for_dotnet_command_line_fn mp_initCmdLineFPtr = nullptr;
    hostfxr_get_runtime_delegate_fn               mp_getDelegateFptr = nullptr;
    hostfxr_close_fn                              mp_closeFptr       = nullptr;
    get_function_pointer_fn                       mp_getFptr         = nullptr;

    //////////////////////////////////////////////////////
    // Map of known functions
    //////////////////////////////////////////////////////
    std::unordered_map<std::wstring, fp_t> m_managedMethodCache;

    //////////////////////////////////////////////////////
    // State
    //////////////////////////////////////////////////////
    bool scriptAssemblyLoaded = false;
};

ScriptEngineInternalData* ScriptEngine::s_data;
ref<Scene>                ScriptEngine::sp_sceneContext = nullptr;

#ifdef NB_WINDOWS
static void* s_loadLibrary(const char_t* path)
{
    HMODULE h = ::LoadLibraryW(path);
    assert(h != nullptr);
    return (void*)h;
}
static void* s_getExport(void* h, const char* name)
{
    void* f = (void*)::GetProcAddress((HMODULE)h, name);
    assert(f != nullptr);
    return f;
}
#else
static void* s_loadLibrary(const char_t* path)
{
    void* h = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
    assert(h != nullptr);
    return h;
}
static void* s_getExport(void* h, const char* name)
{
    void* f = dlsym(h, name);
    assert(f != nullptr);
    return f;
}

#endif

void ScriptEngine::s_init(const std::string& installPath)
{
    static std::once_flag initFlag;
    std::call_once(
        initFlag,
        [=]()
        {
            s_data = new ScriptEngineInternalData();

            NB_UNUSED(installPath);  // TODO use dotnet that isn't pre-installed?

            std::filesystem::path assemblyPath("../resources/scriptCore/bin/scriptCore.dll");
            std::filesystem::path runtimeConfigPath("../resources/scriptCore/bin/scriptCore.runtimeconfig.json");
            std::filesystem::path executablePath(util::getExecutablePath());

            if (!std::filesystem::exists(assemblyPath))
            {
                NB_CORE_ASSERT_STATIC(false, "%ls doesn't exist!", assemblyPath.generic_wstring().c_str());
            }

            if (!std::filesystem::exists(runtimeConfigPath))
            {
                NB_CORE_ASSERT_STATIC(false, "%ls doesn't exist!", runtimeConfigPath.generic_wstring().c_str());
            }

            std::wstring assemblyPathWString      = assemblyPath.generic_wstring();
            std::wstring runtimeConfigPathWString = runtimeConfigPath.generic_wstring();
            std::wstring executablePathWString    = executablePath.parent_path().generic_wstring();

            get_hostfxr_parameters getHostfxrParams;
            getHostfxrParams.size          = sizeof(hostfxr_initialize_parameters);
            getHostfxrParams.assembly_path = assemblyPathWString.c_str();
            getHostfxrParams.dotnet_root   = nullptr;  // search system path for dotnet

            // Pre-allocate a large buffer for the path to hostfxr
            char_t hostFxrPath[MAX_PATH];
            size_t pathBufSz = sizeof(hostFxrPath) / sizeof(char_t);
            int    rc        = get_hostfxr_path(hostFxrPath, &pathBufSz, nullptr);
            NB_CORE_ASSERT_STATIC(!rc, "Could not locate hostfxr lib!");

            Log::coreInfo("hostfxr located at %ls", hostFxrPath);

            // Load hostfxr and get desired exports
            void* lib = s_loadLibrary(hostFxrPath);
            s_data->mp_initRuntimeFptr
                = (hostfxr_initialize_for_runtime_config_fn)s_getExport(lib, "hostfxr_initialize_for_runtime_config");
            s_data->mp_initCmdLineFPtr = (hostfxr_initialize_for_dotnet_command_line_fn)s_getExport(
                lib, "hostfxr_initialize_for_dotnet_command_line");
            s_data->mp_getDelegateFptr
                = (hostfxr_get_runtime_delegate_fn)s_getExport(lib, "hostfxr_get_runtime_delegate");
            s_data->mp_closeFptr = (hostfxr_close_fn)s_getExport(lib, "hostfxr_close");

            NB_CORE_ASSERT_STATIC(s_data->mp_initRuntimeFptr && s_data->mp_initCmdLineFPtr && s_data->mp_getDelegateFptr
                                      && s_data->mp_closeFptr,
                                  "Could not get hostfxr function pointers!");

            // initialize for cmd line
            const char_t*                 argv1   = assemblyPathWString.c_str();
            const char_t*                 argv[1] = {argv1};
            hostfxr_initialize_parameters initParams;

            initParams.size        = sizeof(hostfxr_initialize_parameters);
            initParams.host_path   = assemblyPathWString.c_str();
            initParams.dotnet_root = nullptr;

            hostfxr_handle handle = nullptr;
            rc                    = s_data->mp_initCmdLineFPtr(1, argv, &initParams, &handle);

            NB_CORE_ASSERT_STATIC(!rc && handle, "failed to initialize hostfxr!");

            // get runtime delegate
            void* p_getFptr = nullptr;
            rc              = s_data->mp_getDelegateFptr(handle, hdt_get_function_pointer, &p_getFptr);
            NB_CORE_ASSERT_STATIC(!rc && p_getFptr, "failed to get get runtime delegate hdt_get_function_pointer!");
            s_data->mp_getFptr = (get_function_pointer_fn)(p_getFptr);

            s_data->mp_closeFptr(handle);

            // call init inside managed code
            s_invokeManagedMethodByName<void>(STR("InitializeScriptCore"));

            // print some info to verify all is in order
            ip_t*       p_runtimeInfo  = s_invokeManagedMethodByName<ip_t*>(STR("GetRuntimeInformation"));
            const char* runtimeInfoStr = (char*)p_runtimeInfo;
            Log::coreInfo("Dotnet runtime info %s", runtimeInfoStr);

            s_freeMemory(p_runtimeInfo);
            
            internalCallsInit();

        });
}

void ScriptEngine::s_destroy()
{
    sp_sceneContext = nullptr;
    delete s_data;
}

void ScriptEngine::s_setSceneContext(const ref<Scene>& p_scene)
{
    sp_sceneContext = p_scene;
}

fp_t ScriptEngine::s_getStaticMethodPtr(const std::wstring& name, const std::wstring& typeName)
{
    // try to find the method pointer in the cache
    auto it = s_data->m_managedMethodCache.find(name);

    fp_t fn = nullptr;
    if (it != s_data->m_managedMethodCache.end())
    {
        // we found it, send it
        fn = it->second;
    }
    else
    {
        // didn't find it, try to load it
        const int rc = s_data->mp_getFptr(
            typeName.c_str(), name.c_str(), UNMANAGEDCALLERSONLY_METHOD, nullptr, nullptr, (void**)&fn);
        if (rc != 0)
        {
            Log::coreError("Failed to get pointer to managed function %ls", name.c_str());
        }
        else
        {
            // save it
            s_data->m_managedMethodCache.emplace(name, fn);
        }
    }

    return fn;
}

void ScriptEngine::s_freeMemory(ip_t* p)
{
    if (!p)
    {
        return;
    }

    static fp_t p_fn = s_getStaticMethodPtr(STR("MemFree"));

    s_invokeManagedMethod<void, ip_t*>(p_fn, p);
}

void ScriptEngine::s_releaseHandle(ip_t* h)
{
    if (!h)
    {
        return;
    }

    static fp_t p_fn = s_getStaticMethodPtr(STR("ReleaseHandle"));

    s_invokeManagedMethod<void, ip_t*>(p_fn, h);
}

bool ScriptEngine::s_loadScriptAssembly(const std::filesystem::path& assemblyPath)
{
    static fp_t p_fn = s_getStaticMethodPtr(STR("LoadScriptAssembly"));

    std::filesystem::path fullScriptAssemblyPath = std::filesystem::absolute(assemblyPath);

    if (!std::filesystem::exists(fullScriptAssemblyPath))
    {
        Log::coreError("Script assembly %s not found!", fullScriptAssemblyPath.generic_string().c_str());
        return false;
    }

    std::string scriptAssemblyPathStr = fullScriptAssemblyPath.generic_string();

    s_data->scriptAssemblyLoaded = s_invokeManagedMethod<bool, const char*>(p_fn, scriptAssemblyPathStr.c_str());

    return s_data->scriptAssemblyLoaded;
}

bool ScriptEngine::s_unloadScriptAssembly()
{
    static fp_t p_fn = s_getStaticMethodPtr(STR("UnloadScriptAssembly"));

    if (s_data->scriptAssemblyLoaded)
    {
        s_data->scriptAssemblyLoaded = !s_invokeManagedMethod<bool>(p_fn);
    }

    return !s_data->scriptAssemblyLoaded;
}

bool ScriptEngine::s_reloadScriptAssembly(const std::filesystem::path& assemblyPath)
{
    if (s_unloadScriptAssembly())
    {
        return s_loadScriptAssembly(assemblyPath);
    }
    else
    {
        return false;
    }
}

std::unordered_set<std::string> ScriptEngine::s_getScriptAssemblyTypes(const char* p_baseClassFilter)
{
    static fp_t p_fn = s_getStaticMethodPtr(STR("GetScriptAssemblyTypes"));

    i32_t count = -1;

    ip_t* p_scriptEntityNames = s_invokeManagedMethod<ip_t*, const char*, i32_t*>(p_fn, p_baseClassFilter, &count);

    std::unordered_set<std::string> typeNames;

    for (i32_t i = 0; i < count; i++)
    {
        ip_t*       p_string = *(ip_t**)(p_scriptEntityNames + i);
        const char* p_name   = (const char*)p_string;

        typeNames.insert(p_name);
        s_freeMemory(p_string);
    }

    return typeNames;
}

ref<ScriptEngine::ScriptInstance> ScriptEngine::s_createInstanceOfScriptAssemblyEntity(const std::string& typeName,
                                                                                       u32_t nativeEntityId)
{
    static fp_t p_createInstanceFn     = s_getStaticMethodPtr(STR("CreateInstanceOfScriptAssemblyEntity"));
    static fp_t p_getOnCreateFn        = s_getStaticMethodPtr(STR("GetEntityOnCreateFPtr"));
    static fp_t p_getOnUpdateFn        = s_getStaticMethodPtr(STR("GetEntityOnUpdateFPtr"));
    static fp_t p_getOnPhysicsUpdateFn = s_getStaticMethodPtr(STR("GetEntityOnPhysicsUpdateFPtr"));
    static fp_t p_getOnDestroyFn       = s_getStaticMethodPtr(STR("GetEntityOnDestroyFPtr"));

    // create the instance
    ip_t* p_handle
        = s_invokeManagedMethod<ip_t*, const char*, u32_t>(p_createInstanceFn, typeName.c_str(), nativeEntityId);

    if (p_handle == nullptr)
    {
        Log::coreError("Failed to create instance for ScriptInstance of type %s", typeName.c_str());
        return nullptr;
    }

    // get pointers to the required functions in the instance
    fp_t p_onCreateFn        = s_invokeManagedMethod<ip_t*, ip_t*>(p_getOnCreateFn, p_handle);
    fp_t p_onUpdateFn        = s_invokeManagedMethod<ip_t*, ip_t*>(p_getOnUpdateFn, p_handle);
    fp_t p_onPhysicsUpdateFn = s_invokeManagedMethod<ip_t*, ip_t*>(p_getOnPhysicsUpdateFn, p_handle);
    fp_t p_onDetroyFn        = s_invokeManagedMethod<ip_t*, ip_t*>(p_getOnDestroyFn, p_handle);

    if (p_onCreateFn == nullptr || p_onUpdateFn == nullptr || p_onPhysicsUpdateFn == nullptr || p_onDetroyFn == nullptr)
    {
        Log::coreError("Failed to get function pointers for ScriptInstance of type %s", typeName.c_str());
        return nullptr;
    }

    return ref(new ScriptInstance(p_handle, p_onCreateFn, p_onUpdateFn, p_onPhysicsUpdateFn, p_onDetroyFn));
}

}  // namespace nimbus