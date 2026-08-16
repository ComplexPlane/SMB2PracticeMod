#include "menu_accel.h"

#include "../systems/pref.h"
#include "../utils/mode.h"
#include "../utils/patch.h"
#include "../utils/ppcutil.h"
#include "../utils/relutil.h"

namespace menu_accel {

void accelerate_pause_menu() {
    // nop the instructions that check for R held down to accelerate the menu
    patch::write_nop(relutil::relocate_addr(0x80273478));  // for up inputs
    patch::write_nop(relutil::relocate_addr(0x802735C8));  // for down inputs
}

void apply_vanilla_menu_behavior() {
    // Otherwise, recover the original vanilla instructions
    patch::write_word(relutil::relocate_addr(0x80273478),
                      PPC_INSTR_CMPWI(PPC_R3, 0));  // for up inputs
    patch::write_word(relutil::relocate_addr(0x802735C8),
                      PPC_INSTR_RLWINM(PPC_R3, PPC_R3, 0, 0x16, 0x16, 0));  // for down inputs
}

void tick() {
    u8 pref = pref::get(pref::Pref::MenuAcceleration);
    switch (MenuAccelOptions(pref)) {
        case MenuAccelOptions::AlwaysEnabled: {
            accelerate_pause_menu();
            break;
        }
        case MenuAccelOptions::OnlyInStory: {
            if (mode::is_main_game_mode_story(mkb::main_game_mode)) {
                accelerate_pause_menu();
            } else {
                apply_vanilla_menu_behavior();
            }
            break;
        }
        case MenuAccelOptions::Disabled: {
            apply_vanilla_menu_behavior();
            break;
        }
    }
}

}  // namespace menu_accel