#pragma once

#include "utils/base.h"

void Log__ModAssert(const char *file, s32 line, bool exp);

#define ASSERT(exp) (Log__ModAssert(__FILE__, __LINE__, (exp)))
#define UNREACHABLE()                                                                  \
    ({                                                                                 \
        mkb_OSPanic((char *)__FILE__, __LINE__, (char *)("Invalid codepath reached")); \
        while (true) {                                                                 \
        }                                                                              \
    })
