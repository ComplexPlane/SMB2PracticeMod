#include "banans.h"

#include "patch.h"
#include "pref.h"

namespace banans {

static bool s_prev_enabled = false;

void tick() {
    bool enabled = pref::get_9999_banana_counter();
    if (enabled != s_prev_enabled) {
        s_prev_enabled = enabled;
        if (enabled) {
            patch::write_word(reinterpret_cast<void*>(0x802b8284), 0x2c00270f);
            patch::write_word(reinterpret_cast<void*>(0x802b828c), 0x3800270f);
        } else {
            patch::write_word(reinterpret_cast<void*>(0x802b8284), 0x2c0003e7);
            patch::write_word(reinterpret_cast<void*>(0x802b828c), 0x380003e7);
        }
    }
}

}  // namespace banans