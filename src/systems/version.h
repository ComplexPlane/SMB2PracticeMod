#pragma once

#include "utils/base.h"

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
s32 semver_compare(const SemVer *v1, const SemVer *v2);
s32 pracmod_version_compare(const PracmodVersion *v1, const PracmodVersion *v2);
const char *version_get_str();

#ifndef GIT_HASH
#define GIT_HASH "<unknown>"
#endif
