#include "mods/banans.h"

#include "systems/pref.h"
#include "utils/patch.h"

static bool s_prev_enabled;

void banans_tick() {
    bool enabled = Pref_Get(Pref_BananaCounter9999);
    if (enabled == s_prev_enabled) return;

    s_prev_enabled = enabled;
    if (enabled) {
        patch_write_word((void *)0x802b8284, 0x2c00270f);
        patch_write_word((void *)0x802b828c, 0x3800270f);
    } else {
        patch_write_word((void *)0x802b8284, 0x2c0003e7);
        patch_write_word((void *)0x802b828c, 0x380003e7);
    }
}
