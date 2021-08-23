#include "log.h"

namespace log {

void mod_assert(const char* file, u32 line, bool exp) {
    if (!(exp)) {
        mkb::OSReport("[mod] Failed assertion in %s line %d", file, line);
        while (true)
            ;
    }
}

}  // namespace log