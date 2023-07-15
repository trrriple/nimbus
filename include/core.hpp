#pragma once

#include <memory>
#include "log.hpp"
#include "common.hpp"

#include "SDL.h"

// TODO do this smarter
#define NM_ASSERTS


#define NM_PROFILE_LEVEL_TRACE  3
#define NM_PROFILE_LEVEL_DETAIL 2
#define NM_PROFILE_LEVEL_NORM   1
#define NM_PROFILE_LEVEL_NONE   0

#define NM_PROFILE_LEVEL NM_PROFILE_LEVEL_NONE


////////////////////////////////////////////////////////////////////////////////
// Core Log Interface
////////////////////////////////////////////////////////////////////////////////
#define NM_CORE_INFO(msg, ...) SDL_Log("[CORE] " msg, __VA_ARGS__);

#define NM_CORE_ERROR(msg, ...) \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[CORE] " msg, __VA_ARGS__);

#define NM_CORE_CRITICAL(msg, ...) \
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "[CORE] " msg, __VA_ARGS__);



#ifdef NM_ASSERTS
////////////////////////////////////////////////////////////////////////////////
// Asserts
////////////////////////////////////////////////////////////////////////////////
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
#define NM_CORE_ASSERT_STATIC(condition, msg, ...) \
    {                                              \
        if (!condition)                            \
        {                                          \
            NM_CORE_CRITICAL("[%s:%i] " msg,       \
                             __func__,             \
                             __LINE__,             \
                             __VA_ARGS__);         \
            __debugbreak();                        \
        }                                          \
    }

#else
#define NM_CORE_ASSERT(condition, msg, ...)
#define NM_CORE_ASSERT_STATIC(condition, msg, ...)
#endif /* NM_ASSERTS */

#ifdef NM_PROFILE_LEVEL
////////////////////////////////////////////////////////////////////////////////
// Tracy
////////////////////////////////////////////////////////////////////////////////
#if NM_PROFILE_LEVEL >= NM_PROFILE_LEVEL_NORM
#include "Tracy.hpp"
#define NM_PROFILE_FUNC() ZoneScopedN(__func__)
#define NM_PROFILE() NM_PROFILE_FUNC()
#else
#define NM_PROFILE()
#endif

#if NM_PROFILE_LEVEL >= NM_PROFILE_LEVEL_DETAIL
#define NM_PROFILE_DETAIL() NM_PROFILE_FUNC()
#else
#define NM_PROFILE_DETAIL()
#endif

#if NM_PROFILE_LEVEL >= NM_PROFILE_LEVEL_TRACE
#define NM_PROFILE_TRACE() NM_PROFILE_FUNC()
#else
#define NM_PROFILE_TRACE()
#endif

#else
#define NM_PROFILE()
#define NM_PROFILE_DETAIL()
#define NM_PROFILE_TRACE()
#endif /* NM_PROFILE_LEVEL */


namespace nimbus::core
{
////////////////////////////////////////////////////////////////////////////////
// Common Functions
////////////////////////////////////////////////////////////////////////////////
inline float getTime_s()
{
    return (float)SDL_GetTicks64() / 1000.0f;
}

}  // namespace nimbus::core