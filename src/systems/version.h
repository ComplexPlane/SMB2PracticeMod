#pragma once

#include "utils/base.h"

// Do NOT edit this, it's used directly in the modlink format
typedef struct SemVer {
    u16 major;
    u16 minor;
    u16 patch;
} SemVer;

typedef struct PracmodVersion {
    SemVer semver;
    u16 release_cand;
} PracmodVersion;

extern const PracmodVersion PRACMOD_VERSION;

void version_init();
s32 semver_compare(SemVer *v1, SemVer *v2);
s32 pracmod_version_compare(PracmodVersion *v1, PracmodVersion *v2);
const char *version_get_str();
