#include "mods/banans.h"

#include "systems/pref.h"
#include "utils/patch.h"
#include "utils/relutil.h"

void Banans_Tick() {
    if (Pref_DidChange(Pref_BananaCounter9999)) {
        if (Pref_Get(Pref_BananaCounter9999)) {
            Patch_WriteWord(Rel_RelocateAddr(0x802b8284), 0x2c00270f);
            Patch_WriteWord(Rel_RelocateAddr(0x802b828c), 0x3800270f);
        } else {
            Patch_WriteWord(Rel_RelocateAddr(0x802b8284), 0x2c0003e7);
            Patch_WriteWord(Rel_RelocateAddr(0x802b828c), 0x380003e7);
        }
    }
}
