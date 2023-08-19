#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/core/log.hpp"

#include <cstdarg>

#ifndef NIMBUS_NO_CONSOLE
#include "SDL_log.h"
#endif /* NIMBUS_NO_CONSOLE */

namespace nimbus
{

static const std::string k_infoColor     = "\033[32m";    // green
static const std::string k_warnColor     = "\033[33m";    // green
static const std::string k_errorColor    = "\033[31m";    // red
static const std::string k_criticalColor = "\033[1;31m";  // bold red
static const std::string k_traceColor    = "\033[35m";    // magenta
static const std::string k_clear         = "\033[0m";

void Log::init()
{
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
}

void Log::info(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NM_UNUSED(format);
#else
    va_list args;
    va_start(args, format);

    std::string context = k_infoColor + "[APP] ";
    context += format + k_clear;

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, context.c_str(), args);

    va_end(args);
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::warn(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NM_UNUSED(format);
#else
    va_list args;
    va_start(args, format);

    std::string context = k_warnColor + "[APP] ";
    context += format + k_clear;

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, context.c_str(), args);

    va_end(args);
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::error(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NM_UNUSED(format);
#else
    va_list args;
    va_start(args, format);

    std::string context = k_errorColor + "[APP] ";
    context += format + k_clear;

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, context.c_str(), args);

    va_end(args);
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::critical(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NM_UNUSED(format);
#else
    va_list args;
    va_start(args, format);

    std::string context = k_criticalColor + "[APP] ";
    context += format + k_clear;

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, context.c_str(), args);

    va_end(args);
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::trace(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NM_UNUSED(format);
#else
    va_list args;
    va_start(args, format);

    std::string context = k_traceColor + "[APP] ";
    context += format + k_clear;

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE, context.c_str(), args);

    va_end(args);
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::coreInfo(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NM_UNUSED(format);
#else
    va_list args;
    va_start(args, format);

    std::string context = k_infoColor + "[NIMBUS] ";
    context += format + k_clear;

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, context.c_str(), args);

    va_end(args);
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::coreWarn(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NM_UNUSED(format);
#else
    va_list args;
    va_start(args, format);

    std::string context = k_warnColor + "[NIMBUS] ";
    context += format + k_clear;

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, context.c_str(), args);

    va_end(args);
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::coreError(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NM_UNUSED(format);
#else
    va_list args;
    va_start(args, format);

    std::string context = k_errorColor + "[NIMBUS] ";
    context += format + k_clear;

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, context.c_str(), args);

    va_end(args);
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::coreCritical(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NM_UNUSED(format);
#else
    va_list args;
    va_start(args, format);

    std::string context = k_criticalColor + "[NIMBUS] ";
    context += format + k_clear;

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, context.c_str(), args);

    va_end(args);
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::coreTrace(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NM_UNUSED(format);
#else
    va_list args;
    va_start(args, format);

    std::string context = k_traceColor + "[NIMBUS] ";
    context += format + k_clear;

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE, context.c_str(), args);

    va_end(args);
#endif /* NIMBUS_NO_CONSOLE */
}

}  // namespace nimbus