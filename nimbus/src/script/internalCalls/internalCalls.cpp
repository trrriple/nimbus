#include "nimbus/script/internalCalls/internalCalls.hpp"
#include "nimbus/core/application.hpp"
#include "nimbus/core/common.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#define _CRT_SECURE_NO_WARNINGS
#include "gtx/string_cast.hpp"
#undef _CRT_SECURE_NO_WARNINGS
#include "nimbus.hpp"


namespace nimbus
{


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

INTERNAL_CALL void ic_vec4Test(glm::vec4* vec4)
{
    Log::coreInfo("Got vec %f %f %f %f", vec4->x, vec4->y, vec4->z, vec4->w);
}

INTERNAL_CALL void ic_mat4Test(glm::mat4* mat4)
{
    Log::coreInfo("Got mat4 %s", glm::to_string(*mat4).c_str());

    glm::mat4 test(1.123f,
                   2.234f,
                   3.345f,
                   4.456f,
                   11.123f,
                   12.234f,
                   13.345f,
                   14.456f,
                   21.123f,
                   22.234f,
                   23.345f,
                   24.456f,
                   31.123f,
                   32.234f,
                   33.345f,
                   34.456f);

    Log::coreInfo("Compare-> %s", glm::to_string(test).c_str());

}

}  // namespace nimbus