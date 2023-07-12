#pragma once


#include "nmpch.hpp"

// TODO do this smarter
#define NM_ASSERTS

#define NM_CORE_INFO(msg, ...) SDL_Log("[CORE] " msg, __VA_ARGS__);

#define NM_CORE_ERROR(msg, ...) \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[CORE] " msg, __VA_ARGS__);

#define NM_CORE_CRITICAL(msg, ...) \
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "[CORE] " msg, __VA_ARGS__);

#define NM_INFO(msg, ...) SDL_Log("[APP] " msg, __VA_ARGS__);

#define NM_ERROR(msg, ...) \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[APP] " msg, __VA_ARGS__);

#define NM_CRITICAL(msg, ...) \
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "[APP] " msg, __VA_ARGS__);


#ifdef NM_ASSERTS

#include <typeinfo>
#define NM_CORE_ASSERT(condition, msg, ...)        \
    {                                              \
        if (!condition)                            \
        {                                          \
            NM_CORE_CRITICAL("[%s::%s:%i] " msg,   \
                             typeid(*this).name(), \
                             __func__,             \
                             __LINE__,             \
                             __VA_ARGS__);         \
            __debugbreak();                        \
        }                                          \
    }

#endif

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