#include "banans.h"

#include "patch.h"

namespace banans {

static bool s_visible;

void tick() {}

void set_visible(bool visible) {
    if (visible && !s_visible) {
        // Patch
        patch::write_word(reinterpret_cast<void*>(0x802b8284), 0x2c00270f);
        patch::write_word(reinterpret_cast<void*>(0x802b828c), 0x3800270f);
    } else if (!visible && s_visible) {
        // Unpatch
        patch::write_word(reinterpret_cast<void*>(0x802b8284), 0x2c0003e7);
        patch::write_word(reinterpret_cast<void*>(0x802b828c), 0x380003e7);
    }

    s_visible = visible;
}

bool is_visible() { return s_visible; }

}  // namespace banans