#pragma once

#include "nimbus/core/log.hpp"
#include "nimbus/core/ref.hpp"

#include <memory>
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////
// Generic Macros
////////////////////////////////////////////////////////////////////////////////
#ifndef NM_UNUSED
#define NM_UNUSED(x) (void)(x)
#endif /* NM_UNUSED */

#ifndef STRINGIFY
#define STRINGIFY(x) #x
#endif /* STRINGIFY */

#define NM_RUNTIME_ASSERTS

#ifdef NM_RUNTIME_ASSERTS
////////////////////////////////////////////////////////////////////////////////
// Asserts
////////////////////////////////////////////////////////////////////////////////
#include <typeinfo>
#ifdef __linux__
#include <signal.h>
#define __debugbreak() raise(SIGTRAP)
#endif
#define NM_ASSERT(condition, msg, ...)                                                                              \
    {                                                                                                               \
        if (!(condition))                                                                                           \
        {                                                                                                           \
            Log::critical("[%s::%s:%i] " msg, typeid(*this).name(), __func__, __LINE__ __VA_OPT__(, ) __VA_ARGS__); \
            __debugbreak();                                                                                         \
        }                                                                                                           \
    }

#define NM_ASSERT_STATIC(condition, msg, ...)                                         \
                                                                                      \
    if (!(condition))                                                                 \
    {                                                                                 \
        Log::critical("[%s:%i] " msg, __func__, __LINE__ __VA_OPT__(, ) __VA_ARGS__); \
        __debugbreak();                                                               \
    }

#else
#define NM_CORE_ASSERT(condition, msg, ...)
#define NM_CORE_ASSERT_STATIC(condition, msg, ...)
#endif /* NM_RUNTIME_ASSERTS */

namespace nimbus
{
////////////////////////////////////////////////////////////////////////////////
// Templates to help increase brevity of using smart pointers.
// Credit Hazel
////////////////////////////////////////////////////////////////////////////////
template <typename T>
using scope = std::unique_ptr<T>;
template <typename T, typename... Args>
constexpr scope<T> genScope(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

}  // namespace nimbus
