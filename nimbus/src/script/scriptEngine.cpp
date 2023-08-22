#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/script/scriptEngine.hpp"
#include "nimbus/core/utility.hpp"

#include <filesystem>

#include "nimbus/platform/os/os.hpp"

#ifdef MONO_CLR
#include "mono/jit/jit.h"
#include "mono/metadata/image.h"
#include "mono/metadata/assembly.h"
#else
#include "nethost.h"
#include "coreclr_delegates.h"
#include "hostfxr.h"
#endif

namespace nimbus
{

struct ScriptEngineInternalData
{
#ifdef MONO_CLR
    ///////////////////////////
    // Domain
    ///////////////////////////
    MonoDomain* rootDomain = nullptr;
    MonoDomain* appDomain  = nullptr;

    ///////////////////////////
    // Assembly
    ///////////////////////////
    MonoAssembly* coreAssembly = nullptr;

#else

    hostfxr_initialize_for_runtime_config_fn      mp_initRuntimeFptr = nullptr;
    hostfxr_initialize_for_dotnet_command_line_fn mp_initCmdLineFPtr = nullptr;
    hostfxr_get_runtime_delegate_fn               mp_getDelegateFptr = nullptr;
    hostfxr_close_fn                              mp_closeFptr       = nullptr;
    get_function_pointer_fn                       mp_getFptr         = nullptr;

#endif
};

ScriptEngineInternalData* ScriptEngine::s_data;

#ifdef MONO_CLR

static MonoAssembly* s_loadCSharpAssembly(const std::string& assemblyPath)
{
    if (!std::filesystem::exists(assemblyPath))
    {
        Log::coreError("%s doesn't exist!", assemblyPath.c_str());

        return nullptr;
    }

    u32_t fileSize = 0;
    char* fileData = util::readFileAsBytes(assemblyPath, &fileSize);

    // NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a
    // reference to the assembly
    MonoImageOpenStatus status;
    MonoImage*          image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

    MonoAssembly* assembly = nullptr;
    do
    {
        if (status != MONO_IMAGE_OK)
        {
            const char* errorMessage = mono_image_strerror(status);

            Log::coreError("Could not open c# image: Error %s", errorMessage);

            break;
        }

        assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
        if (status != MONO_IMAGE_OK)
        {
            const char* errorMessage = mono_image_strerror(status);

            Log::coreError("Could not open c# assemmbly: Error %s", errorMessage);

            break;
        }

    } while (0);

    mono_image_close(image);

    delete[] fileData;

    return assembly;
}

static void s_printAssemblyTypes(MonoAssembly* assembly)
{
    MonoImage*           image                = mono_assembly_get_image(assembly);
    const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
    i32_t                numTypes             = mono_table_info_get_rows(typeDefinitionsTable);

    for (i32_t i = 0; i < numTypes; i++)
    {
        u32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
        const char* name      = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

        Log::coreInfo("%s.%s", nameSpace, name);
    }
}

MonoClass* s_getClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className)
{
    MonoImage* image = mono_assembly_get_image(assembly);
    MonoClass* klass = mono_class_from_name(image, namespaceName, className);

    if (klass == nullptr)
    {
        Log::coreError("Could not get class %s in namespace %s in assembly", className, namespaceName);
        return nullptr;
    }

    return klass;
}

#else
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

template <typename rT, typename... Args>
static inline rT s_callManagedMethodByName(const std::wstring& name, Args... args)
{
    typedef rT(CORECLR_DELEGATE_CALLTYPE * fn)(Args...);
    return ((fn)ScriptEngine::s_getStaticMethodPtr(name))(args...);
}

void* ScriptEngine::s_getStaticMethodPtr(const std::wstring& name)
{
    // TODO cache
    void* fn;

    const int rc = s_data->mp_getFptr(
        STR("nimbus.ScriptCore, scriptCore"), name.c_str(), UNMANAGEDCALLERSONLY_METHOD, nullptr, nullptr, &fn);
    if (rc != 0)
    {
        Log::coreError("Failed to get pointer to managed function %s", name.c_str());
    }

    return fn;
}

void ScriptEngine::s_freeMemory(void* p)
{
    if (!p)
    {
        return;
    }

    s_callManagedMethodByName<void, void*>(STR("memFree"), p);
}

void ScriptEngine::s_loadScriptAssembly()
{
    s_callManagedMethodByName<void>(STR("LoadScriptAssembly"));
}

void ScriptEngine::s_unloadScriptAssembly()
{
    s_callManagedMethodByName<void>(STR("UnloadScriptAssembly"));
}

void ScriptEngine::s_reloadScriptAssembly()
{
    s_callManagedMethodByName<void>(STR("ReloadScriptAssembly"));
}

void ScriptEngine::s_testCallScript()
{
    s_callManagedMethodByName<void>(STR("TestCallScript"));
}


#endif

void ScriptEngine::s_init(const std::string& installPath)
{
    static std::once_flag initFlag;
    std::call_once(initFlag,
                   [=]()
                   {
                       s_data = new ScriptEngineInternalData();

#ifdef MONO_CLR

                       std::filesystem::path libPath(installPath);
                       libPath /= "lib";
                       std::filesystem::path etcPath(installPath);
                       etcPath /= "etc";

                       mono_set_dirs(libPath.generic_string().c_str(), etcPath.generic_string().c_str());
                       s_data->rootDomain = mono_jit_init("nbMonoRuntime");

                       NB_CORE_ASSERT_STATIC(s_data->rootDomain, "Mono Jit failed to load!");

                       s_data->appDomain = mono_domain_create_appdomain((char*)"nbAppDomain", nullptr);
                       mono_domain_set(s_data->appDomain, true);

                       // test loading
                       s_data->coreAssembly = s_loadCSharpAssembly("../resources/scriptCore/bin/scriptCore.dll");
                       s_printAssemblyTypes(s_data->coreAssembly);

                       MonoClass* testingClass = s_getClassInAssembly(s_data->coreAssembly, "nimbus", "ScriptCore");

                       // Allocate an instance of our class
                       MonoObject* classInstance = mono_object_new(s_data->appDomain, testingClass);

                       if (classInstance == nullptr)
                       {
                           Log::coreError("Ripperoni Pizza");
                           return;
                       }

                       // Call the parameterless (default) constructor
                       mono_runtime_object_init(classInstance);

                       // Get a reference to the method in the class
                       MonoMethod* method = mono_class_get_method_from_name(testingClass, "PrintFloatVar", 0);

                       if (method == nullptr)
                       {
                           // No method called "PrintFloatVar" with 0 parameters in the class, log error or something
                           return;
                       }

                       // Call the C# method on the objectInstance instance, and get any potential exceptions
                       MonoObject* exception = nullptr;
                       mono_runtime_invoke(method, classInstance, nullptr, &exception);

                       MonoMethod* method2 = mono_class_get_method_from_name(testingClass, "IncrementFloatVar", 1);

                       f32_t value = 5;
                       void* param = &value;
                       mono_runtime_invoke(method2, classInstance, &param, &exception);

                       mono_runtime_invoke(method, classInstance, nullptr, &exception);

#else

    NB_UNUSED(installPath); // TODO use dotnet that isn't pre-installed?

    std::filesystem::path assemblyPath("../resources/scriptCore/bin/scriptCore.dll");
    std::filesystem::path runtimeConfigPath("../resources/scriptCore/bin/scriptCore.runtimeconfig.json");
    std::filesystem::path executablePath(util::getExecutablePath());

    if (!std::filesystem::exists(assemblyPath))
    {
        NB_CORE_ASSERT_STATIC(false, "%s doesn't exist!", assemblyPath.generic_wstring().c_str());
    }

    if (!std::filesystem::exists(runtimeConfigPath))
    {
        NB_CORE_ASSERT_STATIC(false, "%s doesn't exist!", runtimeConfigPath.generic_wstring().c_str());
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
    int    rc          = get_hostfxr_path(hostFxrPath, &pathBufSz, nullptr);
    NB_CORE_ASSERT_STATIC(!rc, "Could not locate hostfxr lib!");

    Log::coreInfo("hostfxr located at %ls", hostFxrPath);

    // Load hostfxr and get desired exports
    void* lib = s_loadLibrary(hostFxrPath);
    s_data->mp_initRuntimeFptr
        = (hostfxr_initialize_for_runtime_config_fn)s_getExport(lib, "hostfxr_initialize_for_runtime_config");
    s_data->mp_initCmdLineFPtr
        = (hostfxr_initialize_for_dotnet_command_line_fn)s_getExport(lib, "hostfxr_initialize_for_dotnet_command_line");
    s_data->mp_getDelegateFptr = (hostfxr_get_runtime_delegate_fn)s_getExport(lib, "hostfxr_get_runtime_delegate");
    s_data->mp_closeFptr       = (hostfxr_close_fn)s_getExport(lib, "hostfxr_close");

    NB_CORE_ASSERT_STATIC(
        s_data->mp_initRuntimeFptr && s_data->mp_initCmdLineFPtr && s_data->mp_getDelegateFptr && s_data->mp_closeFptr,
        "Could not get hostfxr function pointers!");

    // initialize for cmd line
    const char_t*                 argv1               = assemblyPathWString.c_str();
    const char_t*                 argv[1]             = {argv1};
    hostfxr_initialize_parameters initParams;


    initParams.size        = sizeof(hostfxr_initialize_parameters);
    initParams.host_path   = assemblyPathWString.c_str();
    initParams.dotnet_root = nullptr; 

    hostfxr_handle handle = nullptr;
    rc                    = s_data->mp_initCmdLineFPtr(1, argv, &initParams, &handle);

    NB_CORE_ASSERT_STATIC(!rc && handle, "failed to initialize hostfxr!");

    // get runtime delegate
    void * p_getFptr = nullptr;
    rc = s_data->mp_getDelegateFptr(handle, hdt_get_function_pointer, &p_getFptr);
    NB_CORE_ASSERT_STATIC(!rc && p_getFptr, "failed to get get runtime delegate hdt_get_function_pointer!");
    s_data->mp_getFptr = (get_function_pointer_fn)(p_getFptr);

    s_data->mp_closeFptr(handle);
    
    // call init inside managed code
    s_callManagedMethodByName<char*>(STR("InitializeScriptCore"));


    // messin
    char* runtimeInfo = s_callManagedMethodByName<char*>(STR("GetRuntimeInformation"));
    Log::coreInfo("Dotnet runtime info %s", runtimeInfo);

    s_freeMemory(runtimeInfo);

#endif
                   });
}

void ScriptEngine::s_destroy()
{
#ifdef MONO_CLR
    mono_jit_cleanup(s_data->rootDomain);
#endif

    delete s_data;
}

}  // namespace nimbus