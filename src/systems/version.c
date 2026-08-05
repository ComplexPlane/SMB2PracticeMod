#include "version.h"

const PracmodVersion PRACMOD_VERSION = {{1, 1, 0}, 1};

static char s_version_str[16];

void version_init() {
    if (PRACMOD_VERSION.release_cand > 0) {
        mkb_sprintf(s_version_str, "%d.%d.%d-beta%d", PRACMOD_VERSION.semver.major,
                    PRACMOD_VERSION.semver.minor, PRACMOD_VERSION.semver.patch,
                    PRACMOD_VERSION.release_cand);
    } else {
        mkb_sprintf(s_version_str, "%d.%d.%d", PRACMOD_VERSION.semver.major,
                    PRACMOD_VERSION.semver.minor, PRACMOD_VERSION.semver.patch);
    }
}

s32 semver_compare(SemVer *v1, SemVer *v2) {
    if (v1->major < v2->major) return -1;
    if (v1->major > v2->major) return 1;
    if (v1->minor < v2->minor) return -1;
    if (v1->minor > v2->minor) return 1;
    if (v1->patch < v2->patch) return -1;
    if (v1->patch > v2->patch) return 1;

    return 0;
}

s32 pracmod_version_compare(PracmodVersion *v1, PracmodVersion *v2) {
    s32 semver_result = semver_compare(&v1->semver, &v2->semver);
    if (semver_result != 0) return semver_result;

    // RC of 0 means real release, always considered to be newer than a RC version
    if (v1->release_cand == 0 && v2->release_cand > 0) return 1;
    if (v1->release_cand > 0 && v2->release_cand == 0) return -1;
    if (v1->release_cand > 0 && v2->release_cand > 0) {
        if (v1->release_cand > v2->release_cand) return 1;
        if (v1->release_cand < v2->release_cand) return -1;
    }

    return 0;
}

const char *version_get_str() {
    return s_version_str;
}
