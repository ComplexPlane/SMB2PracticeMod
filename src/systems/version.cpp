#include "version.h"

namespace version {

const PracmodVersion PRACMOD_VERSION = {1, 0, 2, 0};

static char s_version_str[16];

void init() {
    if (PRACMOD_VERSION.release_cand > 0) {
        mkb::sprintf(s_version_str, "%d.%d.%d-beta%d", PRACMOD_VERSION.semver.major,
                     PRACMOD_VERSION.semver.minor, PRACMOD_VERSION.semver.patch,
                     PRACMOD_VERSION.release_cand);
    } else {
        mkb::sprintf(s_version_str, "%d.%d.%d", PRACMOD_VERSION.semver.major,
                     PRACMOD_VERSION.semver.minor, PRACMOD_VERSION.semver.patch);
    }
}

s32 compare(const SemVer& v1, const SemVer& v2) {
    if (v1.major < v2.major) return -1;
    if (v1.major > v2.major) return 1;
    if (v1.minor < v2.minor) return -1;
    if (v1.minor > v2.minor) return 1;
    if (v1.patch < v2.patch) return -1;
    if (v1.patch > v2.patch) return 1;

    return 0;
}

s32 compare(const PracmodVersion& v1, const PracmodVersion& v2) {
    s32 semver_compare = compare(v1.semver, v2.semver);
    if (semver_compare != 0) return semver_compare;

    // RC of 0 means real release, always considered to be newer than a RC version
    if (v1.release_cand == 0 && v2.release_cand > 0) return 1;
    if (v1.release_cand > 0 && v2.release_cand == 0) return -1;
    if (v1.release_cand > 0 && v2.release_cand > 0) {
        if (v1.release_cand > v2.release_cand) return 1;
        if (v1.release_cand < v2.release_cand) return -1;
    }

    return 0;
}

const char* get_version_str() {
    return s_version_str;
}

}  // namespace version
