#pragma once

#include <memory>
#include "log.hpp"
#include "common.hpp"

#include "SDL.h"

// TODO do this smarter
#define NM_RUNTIME_ASSERTS


#define NM_PROFILE_LEVEL_TRACE  3
#define NM_PROFILE_LEVEL_DETAIL 2
#define NM_PROFILE_LEVEL_NORM   1
#define NM_PROFILE_LEVEL_NONE   0

#define NM_PROFILE_LEVEL NM_PROFILE_LEVEL_NONE

#ifdef NM_RUNTIME_ASSERTS
////////////////////////////////////////////////////////////////////////////////
// Asserts
////////////////////////////////////////////////////////////////////////////////
#include <typeinfo>
#ifdef __linux__
#include <signal.h>
#define __debugbreak() raise(SIGTRAP)
#endif
#define NM_CORE_ASSERT(condition, msg, ...)        \
{                                                  \
    if (!(condition))                              \
    {                                              \
        Log::coreCritical(                         \
            "[%s::%s:%i] " msg,                    \
            typeid(*this).name(),                  \
            __func__,                              \
            __LINE__ __VA_OPT__(,) __VA_ARGS__);   \
            __debugbreak();                        \
    }                                              \
}

#define NM_CORE_ASSERT_STATIC(condition, msg, ...) \
                                                   \
    if (!(condition))                              \
    {                                              \
        Log::coreCritical(                         \
            "[%s:%i] " msg,                        \
            __func__,                              \
            __LINE__ __VA_OPT__(,) __VA_ARGS__);   \
            __debugbreak();                        \
    }                                              \


#else
#define NM_CORE_ASSERT(condition, msg, ...)
#define NM_CORE_ASSERT_STATIC(condition, msg, ...)
#endif /* NM_RUNTIME_ASSERTS */

// we always want compile time asserts
#define NM_CORE_COMPILETIME_ASSERT(cond, msg) static_assert(cond, msg)

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
#include <chrono>
#include <mutex>



namespace nimbus::core
{
////////////////////////////////////////////////////////////////////////////////
// Common Functions
////////////////////////////////////////////////////////////////////////////////
inline double getTime_s()
{
    static std::mutex mtx;
    static bool       initialized = false;
    static std::chrono::time_point<std::chrono::steady_clock> startTime;

    std::lock_guard<std::mutex> lock(mtx);

    if (!initialized)
    {
        startTime   = std::chrono::steady_clock::now();
        initialized = true;
    }

    // Get the current time point using the steady clock
    auto currentTime = std::chrono::steady_clock::now();

    // Calculate the elapsed time in microseconds
    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(
                           currentTime - startTime)
                           .count();

    // Convert microseconds to seconds (with microsecond precision)
    return static_cast<double>(elapsedTime) * 1E-6;
}

}  // namespace nimbus::core