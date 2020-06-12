#pragma once

#include <cstdint>

namespace main
{

extern "C" {

// Assembly overwrite functions
void CustomTitleScreenTextColor();

// main.cpp
void StartMainLoopAssembly();

// Functions accessed by assembly overwrites
// main.cpp
void tick();

}

}