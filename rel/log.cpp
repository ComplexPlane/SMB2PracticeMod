#include "log.h"

namespace log {

void mod_assert(const char* file, s32 line, bool exp) {
    if (!(exp)) {
        mkb::OSPanic(const_cast<char*>(file), line, "[mod] Assertion failed");
        while (true)
            ;
    }
}

}  // namespace log