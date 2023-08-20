#pragma once

#include "nimbus/core/common.hpp"
#include "nimbus/core/log.hpp"
#include "nimbus/core/stopwatch.hpp"

#include <memory>

// TODO do this smarter
#define NB_RUNTIME_ASSERTS

#define NB_PROFILE_LEVEL_TRACE 3
#define NB_PROFILE_LEVEL_DETAIL 2
#define NB_PROFILE_LEVEL_NORM 1
#define NB_PROFILE_LEVEL_NONE 0

#define NB_PROFILE_LEVEL NB_PROFILE_LEVEL_NONE

#ifdef NB_RUNTIME_ASSERTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Asserts
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <typeinfo>
#ifdef __linux__
#include <signal.h>
#define __debugbreak() raise(SIGTRAP)
#endif
#define NB_CORE_ASSERT(condition, msg, ...)                         \
    {                                                               \
        if (!(condition))                                           \
        {                                                           \
            Log::coreCritical("[%s::%s:%i] " msg,                   \
                              typeid(*this).name(),                 \
                              __func__,                             \
                              __LINE__ __VA_OPT__(, ) __VA_ARGS__); \
            __debugbreak();                                         \
        }                                                           \
    }

#define NB_CORE_ASSERT_STATIC(condition, msg, ...)                                        \
                                                                                          \
    if (!(condition))                                                                     \
    {                                                                                     \
        Log::coreCritical("[%s:%i] " msg, __func__, __LINE__ __VA_OPT__(, ) __VA_ARGS__); \
        __debugbreak();                                                                   \
    }

#else
#define NB_CORE_ASSERT(condition, msg, ...)
#define NB_CORE_ASSERT_STATIC(condition, msg, ...)
#endif /* NB_RUNTIME_ASSERTS */

// we always want compile time asserts
#define NB_CORE_COMPILETIME_ASSERT(cond, msg) static_assert(cond, msg)

#ifdef NB_PROFILE_LEVEL
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tracy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if NB_PROFILE_LEVEL >= NB_PROFILE_LEVEL_NORM
#include "Tracy.hpp"
#define NB_PROFILE_FUNC() ZoneScopedN(__func__)
#define NB_PROFILE() NB_PROFILE_FUNC()
#else
#define NB_PROFILE()
#endif

#if NB_PROFILE_LEVEL >= NB_PROFILE_LEVEL_DETAIL
#define NB_PROFILE_DETAIL() NB_PROFILE_FUNC()
#else
#define NB_PROFILE_DETAIL()
#endif

#if NB_PROFILE_LEVEL >= NB_PROFILE_LEVEL_TRACE
#define NB_PROFILE_TRACE() NB_PROFILE_FUNC()
#else
#define NB_PROFILE_TRACE()
#endif

#else
#define NB_PROFILE()
#define NB_PROFILE_DETAIL()
#define NB_PROFILE_TRACE()
#endif /* NB_PROFILE_LEVEL */
#include <chrono>
#include <mutex>

namespace nimbus::core
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Common Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline f64_t getTime_s()
{
    static std::mutex mtx;

    std::lock_guard<std::mutex> lock(mtx);
    static Stopwatch            sw;

    return sw.elapsed();
}

}  // namespace nimbus::core