#include "banans.h"

#include "systems/pref.h"
#include "utils/patch.h"

namespace banans {

void tick() {
    if (pref::did_change(pref::BoolPref::BananaCounter9999)) {
        if (pref::get(pref::BoolPref::BananaCounter9999)) {
            patch::write_word(reinterpret_cast<void*>(0x802b8284), 0x2c00270f);
            patch::write_word(reinterpret_cast<void*>(0x802b828c), 0x3800270f);
        } else {
            patch::write_word(reinterpret_cast<void*>(0x802b8284), 0x2c0003e7);
            patch::write_word(reinterpret_cast<void*>(0x802b828c), 0x380003e7);
        }
    }
}

}  // namespace banans
