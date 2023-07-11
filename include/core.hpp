#pragma once

#include "nmpch.hpp"

#define NM_LOG SDL_Log
#define NM_ELOG SDL_LogError

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

namespace nimbus::core
{

//////////////////////////////////
// Types
/////////////////////////////////

//////////////////////////////////
// Common Functions
/////////////////////////////////
inline float getTime_s()
{
    return (float)SDL_GetTicks64() / 1000.0f;
}

}  // namespace nimbus::core