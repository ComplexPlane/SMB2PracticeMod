#pragma once

// Necessary to use macros here so file information is accurate
// Note that these macros won't necessarily halt the game in Dolphin as Dolphin lets you ignore assertions
// Prefer to use MOD_ASSERT and friends instead

#define ASSERT(exp)                                             \
    (void) ((exp) ||                                            \
            (gc::OSPanic(__FILE__, __LINE__, "Failed assertion " #exp), 0))

#define ASSERTMSG(exp, msg)                                     \
    (void) ((exp) ||                                            \
            (gc::OSPanic(__FILE__, __LINE__, (msg)), 0))

namespace gc
{

extern "C" {

void OSPanic(const char *file, s32 line, const char *msg, ...);

}

}
