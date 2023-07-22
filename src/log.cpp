#include "log.hpp"

#include <cstdarg>

#include "SDL_log.h"
#include "core.hpp"
#include "nmpch.hpp"

namespace nimbus
{
void Log::init()
{
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
}

void Log::appInfo(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    std::string context = "[APP] ";
    context += format;

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

    std::string context = "[APP] ";
    context += format;

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

    std::string context = "[APP] ";
    context += format;

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

    std::string context = "[APP] ";
    context += format;

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

    std::string context = "[NIMBUS] ";
    context += format;

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

    std::string context = "[NIMBUS] ";
    context += format;

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

    std::string context = "[NIMBUS] ";
    context += format;

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

    std::string context = "[NIMBUS] ";
    context += format;

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION,
                    SDL_LOG_PRIORITY_CRITICAL,
                    context.c_str(),
                    args);

    va_end(args);
}

}  // namespace nimbus