#include "banans.h"

#include "systems/pref.h"
#include "utils/patch.h"
#include "utils/relutil.h"

namespace banans {

static bool s_prev_enabled = false;

void tick() {
    bool enabled = pref::get(pref::Pref::BananaCounter9999);
    if (enabled != s_prev_enabled) {
        s_prev_enabled = enabled;
        if (enabled) {
            patch::write_word(relutil::relocate_addr(0x802b8284), 0x2c00270f);
            patch::write_word(relutil::relocate_addr(0x802b828c), 0x3800270f);
        } else {
            patch::write_word(relutil::relocate_addr(0x802b8284), 0x2c0003e7);
            patch::write_word(relutil::relocate_addr(0x802b828c), 0x380003e7);
        }
    }
}

}  // namespace banans
