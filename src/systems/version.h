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

s32 compare(const SemVer& v1, const SemVer& v2);
s32 compare(const PracmodVersion& v1, const PracmodVersion &v2);
void get_version_str(char *out_str);

}
