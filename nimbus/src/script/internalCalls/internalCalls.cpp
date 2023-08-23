#include "nimbus/script/internalCalls/internalCalls.hpp"
#include "nimbus/core/application.hpp"
#include "nimbus/core/common.hpp"

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

}  // namespace nimbus