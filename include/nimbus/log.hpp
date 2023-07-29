#pragma once

#include "common.hpp"

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
    static void appInfo(const char* format, ...);

    static void appWarn(const char* format, ...);

    static void appError(const char* format, ...);

    static void appCritical(const char* format, ...);

    ////////////////////////////////////////////////////////////////////////////
    // Log a Core message. Ideally don't use these in application
    ////////////////////////////////////////////////////////////////////////////
    static void coreInfo(const char* format, ...);

    static void coreWarn(const char* format, ...);

    static void coreError(const char* format, ...);

    static void coreCritical(const char* format, ...);


};
}  // namespace nimbus
