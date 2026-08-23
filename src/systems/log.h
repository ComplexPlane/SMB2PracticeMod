#pragma once

#include "utils/base.h"

void Log_ModAssert(const char *file, s32 line, bool exp);

#define ASSERT(exp) (Log_ModAssert(__FILE__, __LINE__, (exp)))
#define UNREACHABLE()                                                                    \
    ({                                                                                   \
        mkb_OSPanic((char *)__FILE__, __LINE__, (char *)("Invalid codepath reached"));   \
        while (true) {                                                                   \
        }                                                                                \
    })
