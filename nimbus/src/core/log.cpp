#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/core/log.hpp"

#include <cstdarg>
#include <fstream>

namespace nimbus
{

#define WRITE_LOG(prefixStr, msgType)                                                                \
    va_list args;                                                                                    \
    va_start(args, format);                                                                          \
    LogMessage* logMsg   = _s_getSlot();                                                             \
    char        prefix[] = prefixStr;                                                                \
    strcpy_s(logMsg->msg, prefix);                                                                   \
    logMsg->type = msgType;                                                                          \
    vsnprintf(logMsg->msg + sizeof(prefix) - 1, sizeof(logMsg->msg) - sizeof(prefix), format, args); \
    va_end(args);

struct LogInternalData
{
    Log::LogData  logData;
    std::ofstream outFile;

    u32_t logHistory = 10000;
};

LogInternalData* Log::sp_data;

void Log::s_init()
{
    sp_data = new LogInternalData();

    sp_data->logData.msgs.resize(sp_data->logHistory);

    sp_data->outFile = std::ofstream("log.txt");
}

void Log::s_destroy()
{
    sp_data->outFile.close();

    delete sp_data;
}

Log::LogData& Log::s_getLogData()
{
    return sp_data->logData;
}

Log::LogMessage* Log::_s_getSlot()
{
    u32_t idx;

    if (sp_data->logData.tail < sp_data->logHistory - 1)
    {
        idx = sp_data->logData.tail++;
    }
    else
    {
        sp_data->logData.tail = 0;
        idx                   = sp_data->logHistory - 1;
    }

    if (sp_data->logData.quantity < sp_data->logHistory)
    {
        sp_data->logData.quantity++;
    }

    return &sp_data->logData.msgs[idx];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Application Log
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Log::info(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else

    WRITE_LOG("[Appl][Info] ", Type::info);

#endif /* NIMBUS_NO_CONSOLE */
}

void Log::warn(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    WRITE_LOG("[Appl][Warn] ", Type::warn);

#endif /* NIMBUS_NO_CONSOLE */
}

void Log::error(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else

    WRITE_LOG("[Appl][Erro] ", Type::error);

#endif /* NIMBUS_NO_CONSOLE */
}

void Log::critical(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    WRITE_LOG("[Appl][Crit] ", Type::critical);

#endif /* NIMBUS_NO_CONSOLE */
}

void Log::trace(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    WRITE_LOG("[Appl][Trce] ", Type::trace);

#endif /* NIMBUS_NO_CONSOLE */
}

//////////////////////////////////////////////////////
// Application Script Log
//////////////////////////////////////////////////////
void Log::scriptInfo(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    WRITE_LOG("[ScrA][Info] ", Type::info);

#endif /* NIMBUS_NO_CONSOLE */
}

void Log::scriptWarn(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    WRITE_LOG("[ScrA][Warn] ", Type::warn);

#endif /* NIMBUS_NO_CONSOLE */
}

void Log::scriptError(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    WRITE_LOG("[ScrA][Erro] ", Type::error);

#endif /* NIMBUS_NO_CONSOLE */
}

void Log::scriptCritical(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    WRITE_LOG("[ScrA][Crit] ", Type::critical);

#endif /* NIMBUS_NO_CONSOLE */
}

void Log::scriptTrace(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    WRITE_LOG("[ScrA][Trce] ", Type::trace);

#endif /* NIMBUS_NO_CONSOLE */
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Core Log
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Log::coreInfo(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    WRITE_LOG("[Core][Info] ", Type::info);

#endif /* NIMBUS_NO_CONSOLE */
}

void Log::coreWarn(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    WRITE_LOG("[Core][Warn] ", Type::warn);

#endif /* NIMBUS_NO_CONSOLE */
}

void Log::coreError(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    WRITE_LOG("[Core][Erro] ", Type::error);

#endif /* NIMBUS_NO_CONSOLE */
}

void Log::coreCritical(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    WRITE_LOG("[Core][Crit] ", Type::critical);

#endif /* NIMBUS_NO_CONSOLE */
}

void Log::coreTrace(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    WRITE_LOG("[Core][Trce] ", Type::trace);

#endif /* NIMBUS_NO_CONSOLE */
}

//////////////////////////////////////////////////////
// Core Script Log
//////////////////////////////////////////////////////
void Log::scriptCoreInfo(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    WRITE_LOG("[ScrC][Info] ", Type::info);

#endif /* NIMBUS_NO_CONSOLE */
}

void Log::scriptCoreWarn(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    WRITE_LOG("[ScrC][Warn] ", Type::warn);

#endif /* NIMBUS_NO_CONSOLE */
}

void Log::scriptCoreError(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    WRITE_LOG("[ScrC][Erro] ", Type::error);

#endif /* NIMBUS_NO_CONSOLE */
}

void Log::scriptCoreCritical(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    WRITE_LOG("[ScrC][Crit] ", Type::critical);

#endif /* NIMBUS_NO_CONSOLE */
}

void Log::scriptCoreTrace(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    WRITE_LOG("[ScrC][Trce] ", Type::trace);

#endif /* NIMBUS_NO_CONSOLE */
}

}  // namespace nimbus