#pragma once

#include "nimbus/core/common.hpp"

#include <string>

////////////////////////////////////////////////////////////////////////////////
// Log Interface
////////////////////////////////////////////////////////////////////////////////

namespace nimbus
{

class Log
{
   public:
    static void init() noexcept;
    
    ////////////////////////////////////////////////////////////////////////////
    // Log a App message. Ideally don't use these in core
    ////////////////////////////////////////////////////////////////////////////
    static void info(const char* format, ...) noexcept;

    static void warn(const char* format, ...) noexcept;

    static void error(const char* format, ...) noexcept;

    static void critical(const char* format, ...) noexcept;
    
    static void trace(const char* format, ...) noexcept;


    ////////////////////////////////////////////////////////////////////////////
    // Log a Core message. Ideally don't use these in application
    ////////////////////////////////////////////////////////////////////////////
    static void coreInfo(const char* format, ...) noexcept;

    static void coreWarn(const char* format, ...) noexcept;

    static void coreError(const char* format, ...) noexcept;

    static void coreCritical(const char* format, ...) noexcept;

    static void coreTrace(const char* format, ...) noexcept;


};
}  // namespace nimbus
