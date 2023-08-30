#pragma once

#include "nimbus/core/common.hpp"
#include <string>
#include <vector>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Log Interface
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace nimbus
{

struct LogInternalData;

class NIMBUS_API Log
{
   public:
    inline static const u32_t k_maxLogMsgLen = 512;

    enum class Type
    {
        info,
        warn,
        error,
        critical,
        trace
    };

    struct LogMessage
    {
        Type type;
        char msg[512];
    };

    struct LogData
    {
        std::vector<LogMessage> msgs;
        std::atomic<u32_t>      tail;
        std::atomic<u32_t>      quantity;
    };

    static void     s_init();
    static void     s_destroy();
    static LogData& s_getLogData();

    // Note: typical s_ removed for brevity
    ////////////////////////////////////////////////////////////////////////////
    // Log a App message. Ideally don't use these in core
    ////////////////////////////////////////////////////////////////////////////
    static void info(const char* format, ...);

    static void warn(const char* format, ...);

    static void error(const char* format, ...);

    static void critical(const char* format, ...);

    static void trace(const char* format, ...);

    ///////////////////////////
    // Script
    ///////////////////////////
    static void scriptInfo(const char* format, ...);

    static void scriptWarn(const char* format, ...);

    static void scriptError(const char* format, ...);

    static void scriptCritical(const char* format, ...);

    static void scriptTrace(const char* format, ...);

    ////////////////////////////////////////////////////////////////////////////
    // Log a Core message. Ideally don't use these in application
    ////////////////////////////////////////////////////////////////////////////
    static void coreInfo(const char* format, ...);

    static void coreWarn(const char* format, ...);

    static void coreError(const char* format, ...);

    static void coreCritical(const char* format, ...);

    static void coreTrace(const char* format, ...);


    ///////////////////////////
    // Script
    ///////////////////////////
    static void scriptCoreInfo(const char* format, ...);

    static void scriptCoreWarn(const char* format, ...);

    static void scriptCoreError(const char* format, ...);

    static void scriptCoreCritical(const char* format, ...);

    static void scriptCoreTrace(const char* format, ...);

   private:
    static LogInternalData* sp_data;

    static LogMessage* _s_getSlot(u32_t& idx);
};
}  // namespace nimbus
