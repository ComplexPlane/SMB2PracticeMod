#pragma once

#include "mkb/mkb.h"

namespace version {

struct SemVer {
    u16 major;
    u16 minor;
    u16 patch;
};

extern const SemVer PRACMOD_VERSION;

s32 compare(const SemVer& v1, const SemVer& v2);

}
