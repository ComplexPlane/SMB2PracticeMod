#include "log.h"

#include <cstdarg>

namespace log {

void mod_assert(const char* file, s32 line, bool exp) {
    if (!(exp)) {
        mkb::OSPanic(const_cast<char*>(file), line, "[wsmod] Assertion failed");
        while (true)
            ;
    }
}

[[noreturn]] void abort() {
    mkb::OSReport("[wsmod] Aborted. See above for error messages.\n");
    while (true)
        ;
}

[[noreturn]] void abort(const char *format, ...) {
    va_list args;
    va_start(args, format);
    mkb::vprintf(const_cast<char*>(format), args);
    va_end(args);

    while (true)
        ;
}

}  // namespace log
