#pragma once

#include <mkb.h>

// These seem terribly hacky, maybe a better replacement could be made in the future
// Maybe we could even show a custom crash screen!

#define MOD_ASSERT(exp) \
({ \
    if (!(exp)) \
    { \
        mkb::OSPanic(__FILE__, __LINE__, "Failed assertion " #exp); \
        mkb::OSReport("[mod] Failed assertion in %s line %d: %s\n", __FILE__, __LINE__, #exp); \
        while (true); \
    } \
})

#define MOD_ASSERT_MSG(exp, msg) \
({ \
    if (!(exp)) \
    { \
        mkb::OSPanic(__FILE__, __LINE__, msg); \
        mkb::OSReport("[mod] Failed assertion in %s line %d: %s\n", __FILE__, __LINE__, (msg)); \
        while (true); \
    } \
})
