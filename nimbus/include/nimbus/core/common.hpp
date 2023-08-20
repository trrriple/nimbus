#pragma once

#include "nimbus/core/types.hpp"
#include "nimbus/core/log.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Generic Macros
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef NB_UNUSED
#define NB_UNUSED(x) (void)(x)
#endif /* NB_UNUSED */

#ifndef STRINGIFY
#define STRINGIFY(x) #x
#endif /* STRINGIFY */

#define NB_RUNTIME_ASSERTS

#ifdef NB_RUNTIME_ASSERTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Asserts
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <typeinfo>
#ifdef __linux__
#include <signal.h>
#define __debugbreak() raise(SIGTRAP)
#endif
#define NB_ASSERT(condition, msg, ...)                                                                              \
    {                                                                                                               \
        if (!(condition))                                                                                           \
        {                                                                                                           \
            Log::critical("[%s::%s:%i] " msg, typeid(*this).name(), __func__, __LINE__ __VA_OPT__(, ) __VA_ARGS__); \
            __debugbreak();                                                                                         \
        }                                                                                                           \
    }

#define NB_ASSERT_STATIC(condition, msg, ...)                                         \
                                                                                      \
    if (!(condition))                                                                 \
    {                                                                                 \
        Log::critical("[%s:%i] " msg, __func__, __LINE__ __VA_OPT__(, ) __VA_ARGS__); \
        __debugbreak();                                                               \
    }

#else
#define NB_CORE_ASSERT(condition, msg, ...)
#define NB_CORE_ASSERT_STATIC(condition, msg, ...)
#endif /* NB_RUNTIME_ASSERTS */