#include "log.hpp"

#include <cstdarg>

#include "SDL_log.h"
#include "core.hpp"
#include "nmpch.hpp"

namespace nimbus
{

static const std::string k_infoColor     = "\033[32m";    // green
static const std::string k_warnColor     = "\033[33m";    // green
static const std::string k_errorColor    = "\033[31m";    // red
static const std::string k_criticalColor = "\033[1;31m";  // bold red
static const std::string k_clear         = "\033[0m";

void Log::init()
{
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
}

void Log::appInfo(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    std::string context = k_infoColor + "[APP] ";
    context += format + k_clear;

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION,
                    SDL_LOG_PRIORITY_INFO,
                    context.c_str(),
                    args);

    va_end(args);
}

void Log::appWarn(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    std::string context = k_warnColor + "[APP] ";
    context += format + k_clear;

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION,
                    SDL_LOG_PRIORITY_WARN,
                    context.c_str(),
                    args);

    va_end(args);
}

void Log::appError(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    std::string context = k_errorColor + "[APP] ";
    context += format + k_clear;

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION,
                    SDL_LOG_PRIORITY_ERROR,
                    context.c_str(),
                    args);

    va_end(args);
}

void Log::appCritical(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    std::string context = k_criticalColor + "[APP] ";
    context += format + k_clear;

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION,
                    SDL_LOG_PRIORITY_CRITICAL,
                    context.c_str(),
                    args);

    va_end(args);
}

void Log::coreInfo(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    std::string context = k_infoColor + "[NIMBUS] ";
    context += format + k_clear;

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION,
                    SDL_LOG_PRIORITY_INFO,
                    context.c_str(),
                    args);

    va_end(args);
}

void Log::coreWarn(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    std::string context = k_warnColor + "[NIMBUS] ";
    context += format + k_clear;

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION,
                    SDL_LOG_PRIORITY_WARN,
                    context.c_str(),
                    args);

    va_end(args);
}

void Log::coreError(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    std::string context = k_errorColor + "[NIMBUS] ";
    context += format + k_clear;

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION,
                    SDL_LOG_PRIORITY_ERROR,
                    context.c_str(),
                    args);

    va_end(args);
}

void Log::coreCritical(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    std::string context = k_criticalColor + "[NIMBUS] ";
    context += format + k_clear;

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION,
                    SDL_LOG_PRIORITY_CRITICAL,
                    context.c_str(),
                    args);

    va_end(args);
}

}  // namespace nimbus