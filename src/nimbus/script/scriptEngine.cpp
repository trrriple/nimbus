#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/script/scriptEngine.hpp"
#include "nimbus/core/utility.hpp"

#include <filesystem>

#include "mono/jit/jit.h"
#include "mono/metadata/image.h"
#include "mono/metadata/assembly.h"

namespace nimbus
{

struct ScriptEngineInternalData
{
    ///////////////////////////
    // Domain
    ///////////////////////////
    MonoDomain* rootDomain;
    MonoDomain* appDomain;
};

ScriptEngineInternalData ScriptEngine::s_data;

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

void ScriptEngine::s_init(const std::string& installPath)
{
    std::filesystem::path libPath(installPath);
    libPath /= "lib";
    std::filesystem::path etcPath(installPath);
    etcPath /= "etc";

    mono_set_dirs(libPath.generic_string().c_str(), etcPath.generic_string().c_str());
    s_data.rootDomain = mono_jit_init("nbMonoRuntime");

    NB_CORE_ASSERT_STATIC(s_data.rootDomain, "Mono Jit failed to load!");

    s_data.appDomain = mono_domain_create_appdomain((char*)"nbAppDomain", nullptr);
    mono_domain_set(s_data.appDomain, true);

    // test loading
    s_printAssemblyTypes(s_loadCSharpAssembly("CSharpTesting/bin/debug/net7.0/CSharpTesting.dll"));
}

void ScriptEngine::s_destroy()
{
    mono_jit_cleanup(s_data.rootDomain);
}

}  // namespace nimbus