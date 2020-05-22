#pragma once

// Necessary to use macros here so file information is accurate

#define ASSERT(exp)                                             \
    (void) ((exp) ||                                            \
            (gc::OSPanic(__FILE__, __LINE__, "Failed assertion " #exp), 0))

#define ASSERTMSG(exp, msg)                                     \
    (void) ((exp) ||                                            \
            (gc::OSPanic(__FILE__, __LINE__, (msg)), 0))

namespace gc {

extern "C" {

void OSPanic(const char *file, int line, const char *msg, ...);

}

}
