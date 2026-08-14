#pragma once

#include "mkb/mkb.h"

namespace log {
void mod_assert(const char *file, s32 line, bool exp);
}

#define ASSERT(exp) (log::mod_assert(__FILE__, __LINE__, (exp)))
#define UNREACHABLE()                                                                   \
    ({                                                                                  \
        mkb::OSPanic((char *)__FILE__, __LINE__, (char *)("Invalid codepath reached")); \
        while (true) {                                                                  \
        }                                                                               \
    })
