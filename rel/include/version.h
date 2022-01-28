#pragma once

#include <mkb.h>

namespace version {

struct SemVer {
    u16 major;
    u16 minor;
    u16 patch;
};

extern const SemVer PRACMOD_VERSION;

}
