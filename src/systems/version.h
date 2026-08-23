#pragma once

#include "utils/base.h"

typedef struct {
    u16 major;
    u16 minor;
    u16 patch;
} SemVer;

typedef struct {
    SemVer semver;
    u16 release_cand;
} PracmodVersion;

extern const PracmodVersion PRACMOD_VERSION;

void Version_Init();
s32 Version_CompareSemVer(const SemVer *v1, const SemVer *v2);
s32 Version_Compare(const PracmodVersion *v1, const PracmodVersion *v2);
const char *Version_GetStr();

#ifndef GIT_HASH
#define GIT_HASH "<unknown>"
#endif
