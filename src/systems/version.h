#pragma once

#include "mkb/mkb.h"

namespace version {

// Do NOT edit this, it's used directly in the modlink format
struct SemVer {
    u16 major;
    u16 minor;
    u16 patch;
};

struct PracmodVersion {
    SemVer semver;
    u16 release_cand;
};

extern const PracmodVersion PRACMOD_VERSION;

void init();
s32 compare(const SemVer& v1, const SemVer& v2);
s32 compare(const PracmodVersion& v1, const PracmodVersion &v2);
const char *get_version_str();

}
