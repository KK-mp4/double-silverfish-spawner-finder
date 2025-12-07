#pragma once

#include <fstream>
#include <mutex>
#include <vector>

extern "C"
{
#include "finders.h"
#include "generator.h"
}

// Finder settings
// inline constexpr int MC_VERSIONS[2] = {MC_1_8_9, MC_1_9};
inline constexpr int MC_VERSIONS[2] = {MC_1_6_4, MC_1_7};
inline constexpr int PRINT_PROGRESS_EVERY_SEEDS = 10000;
inline constexpr int THRESHOLD = 32;
inline constexpr int THRESHOLD_SQ = THRESHOLD * THRESHOLD;

struct Stronghold {
    unsigned int version;
    Pos pos;
};
