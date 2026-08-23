#pragma once

#include "utils/base.h"

void log_mod_assert(const char *file, s32 line, bool exp);

#define ASSERT(exp) (log_mod_assert(__FILE__, __LINE__, (exp)))
#define UNREACHABLE()                                                                    \
    ({                                                                                   \
        mkb_OSPanic((char *)__FILE__, __LINE__, (char *)("Invalid codepath reached"));   \
        while (true) {                                                                   \
        }                                                                                \
    })
