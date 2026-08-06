#include "log.h"


void log_mod_assert(const char* file, s32 line, bool exp) {
    if (!(exp)) {
        mkb_OSPanic((char *)file, line, "[pracmod] Assertion failed");
        while (true)
            ;
    }
}
