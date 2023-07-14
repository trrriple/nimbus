#pragma once

#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Generic Macros
////////////////////////////////////////////////////////////////////////////////
#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif /* UNUSED */

#ifndef STRINGIFY
#define STRINGIFY(x) #x
#endif /* STRINGIFY */


namespace nimbus
{
////////////////////////////////////////////////////////////////////////////////
// Templates to help increase brevity of using smart pointers.
// Credit Hazel
////////////////////////////////////////////////////////////////////////////////
template <typename T>
using scope = std::unique_ptr<T>;
template <typename T, typename... Args>
constexpr scope<T> makeScope(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using ref = std::shared_ptr<T>;
template <typename T, typename... Args>
constexpr ref<T> makeRef(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

}  // namespace nimbus

