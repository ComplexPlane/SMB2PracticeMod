#include "version.h"

namespace version {

const SemVer PRACMOD_VERSION = {0, 5, 1, 0};

s32 compare(const SemVer& v1, const SemVer& v2) {
    if (v1.major < v2.major) return -1;
    if (v1.major > v2.major) return 1;
    if (v1.minor < v2.minor) return -1;
    if (v1.minor > v2.minor) return 1;
    if (v1.patch < v2.patch) return -1;
    if (v1.patch > v2.patch) return 1;

    // RC of 0 means real release, always considered to be newer than a RC version
    if (v1.release_cand == 0 && v2.release_cand > 0) return 1;
    if (v1.release_cand > 0 && v2.release_cand == 0) return -1;
    if (v1.release_cand > 0 && v2.release_cand > 0) {
        if (v1.release_cand > v2.release_cand) return 1;
        if (v1.release_cand < v2.release_cand) return -1;
    }

    return 0;
}

}
