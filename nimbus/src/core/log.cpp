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


std::string Log::s_scratch;

inline static std::string& s_assemble(std::string& context, const char* format)
{
    context += format + k_clear;

    return context;
}

void Log::init()
{
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
    s_scratch.reserve(2048);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Application Log
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Log::info(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    va_list args;
    va_start(args, format);
    s_scratch = k_infoColor + "[App] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::warn(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    va_list args;
    va_start(args, format);
    s_scratch = k_warnColor + "[App] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::error(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    va_list args;
    va_start(args, format);
    s_scratch = k_errorColor + "[App] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::critical(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    va_list args;
    va_start(args, format);
    s_scratch = k_criticalColor + "[App] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::trace(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    va_list args;
    va_start(args, format);
    s_scratch = k_traceColor + "[App] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
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
    va_list args;
    va_start(args, format);
    s_scratch = k_infoColor + "[Script] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::scriptWarn(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    va_list args;
    va_start(args, format);
    s_scratch = k_warnColor + "[Script] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::scriptError(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    va_list args;
    va_start(args, format);
    s_scratch = k_errorColor + "[Script] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::scriptCritical(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    va_list args;
    va_start(args, format);
    s_scratch = k_criticalColor + "[Script] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::scriptTrace(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    va_list args;
    va_start(args, format);
    s_scratch = k_traceColor + "[Script] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
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
    va_list args;
    va_start(args, format);
    s_scratch = k_infoColor + "[Core] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::coreWarn(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    va_list args;
    va_start(args, format);
    s_scratch = k_warnColor + "[Core] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::coreError(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    va_list args;
    va_start(args, format);
    s_scratch = k_errorColor + "[Core] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::coreCritical(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    va_list args;
    va_start(args, format);
    s_scratch = k_criticalColor + "[Core] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::coreTrace(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    va_list args;
    va_start(args, format);
    s_scratch = k_traceColor + "[Core] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
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
    va_list args;
    va_start(args, format);
    s_scratch = k_infoColor + "[ScriptCore] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::scriptCoreWarn(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    va_list args;
    va_start(args, format);
    s_scratch = k_warnColor + "[ScriptCore] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::scriptCoreError(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    va_list args;
    va_start(args, format);
    s_scratch = k_errorColor + "[ScriptCore] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::scriptCoreCritical(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    va_list args;
    va_start(args, format);
    s_scratch = k_criticalColor + "[ScriptCore] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
#endif /* NIMBUS_NO_CONSOLE */
}

void Log::scriptCoreTrace(const char* format, ...)
{
#ifdef NIMBUS_NO_CONSOLE
    NB_UNUSED(format);
#else
    va_list args;
    va_start(args, format);
    s_scratch = k_traceColor + "[ScriptCore] ";
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE, s_assemble(s_scratch, format).c_str(), args);
    va_end(args);
    s_scratch.clear();
#endif /* NIMBUS_NO_CONSOLE */
}


}  // namespace nimbus