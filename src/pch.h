#pragma once

#ifdef ENABLE_SKYRIM_VR
#undef ENABLE_SKYRIM_VR
#endif

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#define NOMINMAX
#define NOGDI
#include <windows.h>
#include "detours/detours.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;
