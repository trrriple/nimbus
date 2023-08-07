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
    static void init();
    
    ////////////////////////////////////////////////////////////////////////////
    // Log a App message. Ideally don't use these in core
    ////////////////////////////////////////////////////////////////////////////
    static void info(const char* format, ...);

    static void warn(const char* format, ...);

    static void error(const char* format, ...);

    static void critical(const char* format, ...);
    
    static void trace(const char* format, ...);


    ////////////////////////////////////////////////////////////////////////////
    // Log a Core message. Ideally don't use these in application
    ////////////////////////////////////////////////////////////////////////////
    static void coreInfo(const char* format, ...);

    static void coreWarn(const char* format, ...);

    static void coreError(const char* format, ...);

    static void coreCritical(const char* format, ...);

    static void coreTrace(const char* format, ...);


};
}  // namespace nimbus
