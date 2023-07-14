#pragma once

#include "SDL_log.h"


////////////////////////////////////////////////////////////////////////////////
// Log Interface
////////////////////////////////////////////////////////////////////////////////
#define NM_INFO(msg, ...) SDL_Log("[APP] " msg, __VA_ARGS__);

#define NM_ERROR(msg, ...) \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[APP] " msg, __VA_ARGS__);

#define NM_CRITICAL(msg, ...) \
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "[APP] " msg, __VA_ARGS__);

