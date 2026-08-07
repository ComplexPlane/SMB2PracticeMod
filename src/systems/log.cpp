#include "log.h"

namespace log {

void mod_assert(const char* file, s32 line, bool exp) {
    if (!(exp)) {
        mkb::OSPanic((char*)(file), line, (char*)"[pracmod] Assertion failed");
        while (true);
    }
}

}  // namespace log
