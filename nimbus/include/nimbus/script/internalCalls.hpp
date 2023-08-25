#pragma once
#include "nimbus/platform/os/export.h"

#define INTERNAL_CALL extern "C" NIMBUS_API

namespace nimbus
{

void internalCallsInit();

}