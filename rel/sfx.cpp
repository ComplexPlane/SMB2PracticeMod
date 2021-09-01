#include "sfx.h"

#include <mkb.h>
#include "patch.h"
#include "pref.h"

namespace sfx {

void init() {
    if (pref::get_mute_bgm()) {
        patch::write_blr(reinterpret_cast<void*>(mkb::SoftStreamStart));
    }
}

void reboot() {

}

}  // namespace sfx
