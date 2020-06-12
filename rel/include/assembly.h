#pragma once

#include <gc/gc.h>
#include <cstdint>

namespace main
{

extern "C" {

// Assembly overwrite functions
void CustomTitleScreenTextColor();
void FullDebugTextColor();

// main.cpp
void StartMainLoopAssembly();

// Functions accessed by assembly overwrites
// main.cpp
void tick();

// Full color to draw debug text in, if not zero
// Normally, the debug text drawing function uses a single byte for the color in the form RRGGBBAA
extern gc::GXColor debugTextColor;

}

}