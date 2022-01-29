#include "version.h"

namespace version {

const SemVer PRACMOD_VERSION = {0, 5, 1};

s32 compare(const SemVer& v1, const SemVer& v2) {
    if (v1.major < v2.major) return -1;
    if (v1.major > v2.major) return 1;
    if (v1.minor < v2.minor) return -1;
    if (v1.minor > v2.minor) return 1;
    if (v1.patch < v2.patch) return -1;
    if (v1.patch > v2.patch) return 1;
    return 0;
}

}
