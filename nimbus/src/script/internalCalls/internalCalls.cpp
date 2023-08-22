#include "nimbus/script/internalCalls/internalCalls.hpp"
#include "nimbus/core/application.hpp"
#include "nimbus/core/common.hpp"

namespace nimbus
{


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Logging
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INTERNAL_CALL void ic_coreInfo(char* msg)
{
    Log::coreInfo(msg);
    f32_t fps = Application::s_get().getWindow().m_fps;
    Application* app = &Application::s_get();
    Log::coreInfo("FPS %f", fps);
}

INTERNAL_CALL void ic_coreWarn(char* msg)
{
    Log::coreWarn(msg);
}

INTERNAL_CALL void ic_coreError(char* msg)
{
    Log::coreError(msg);
}

INTERNAL_CALL void ic_coreCritical(char* msg)
{
    Log::coreCritical(msg);
}

INTERNAL_CALL void ic_coreTrace(char* msg)
{
    Log::coreTrace(msg);
}

INTERNAL_CALL void ic_info(char* msg)
{
    Log::info(msg);
}

INTERNAL_CALL void ic_warn(char* msg)
{
    Log::warn(msg);
}

INTERNAL_CALL void ic_error(char* msg)
{
    Log::error(msg);
}

INTERNAL_CALL void ic_critical(char* msg)
{
    Log::critical(msg);
}

INTERNAL_CALL void ic_trace(char* msg)
{
    Log::trace(msg);
}

INTERNAL_CALL void ic_test()
{

}

}  // namespace nimbus