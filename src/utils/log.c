#include "utils/log.h"

void Log_ModAssert(const char *file, s32 line, bool exp) {
    if (!(exp)) {
        mkb_OSPanic((char *)(file), line, (char *)"[pracmod] Assertion failed");
        while (true)
            ;
    }
}
