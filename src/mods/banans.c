#include "mods/banans.h"

#include "systems/pref.h"
#include "utils/patch.h"
#include "utils/relutil.h"

void banans_tick() {
    if (Pref_DidChange(Pref_BananaCounter9999)) {
        if (Pref_Get(Pref_BananaCounter9999)) {
            patch_write_word(Rel_RelocateAddr(0x802b8284), 0x2c00270f);
            patch_write_word(Rel_RelocateAddr(0x802b828c), 0x3800270f);
        } else {
            patch_write_word(Rel_RelocateAddr(0x802b8284), 0x2c0003e7);
            patch_write_word(Rel_RelocateAddr(0x802b828c), 0x380003e7);
        }
    }
}
