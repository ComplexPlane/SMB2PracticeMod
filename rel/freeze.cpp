#include "freeze.h"

#include <mkb.h>
#include "patch.h"
#include "pref.h"

namespace freeze {

static bool s_prev_enabled = false;

constexpr u32 TIMER_SUB_ADDR = 0x80297534;
constexpr u32 TIMER_FREEZE_INSTR = 0x7c030214;
constexpr u32 TIMER_SUB_INSTR = 0x3803ffff;

void tick() {
    bool enabled = pref::get_freeze_timer();
    if (enabled != s_prev_enabled) {
        s_prev_enabled = enabled;

        if (enabled) {
            // Disable timer subtraction
            patch::write_word(reinterpret_cast<void*>(TIMER_SUB_ADDR), TIMER_FREEZE_INSTR);
        } else {
            // Enable timer subtraction
            patch::write_word(reinterpret_cast<void*>(TIMER_SUB_ADDR), TIMER_SUB_INSTR);
        }
    }
}

}  // namespace freeze