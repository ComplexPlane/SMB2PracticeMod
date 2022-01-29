#include <mkb.h>

#include "assembly.h"
#include "cmseg.h"
#include "draw.h"
#include "gotostory.h"
#include "heap.h"
#include "inputdisp.h"
#include "iw.h"
#include "jump.h"
#include "menu_impl.h"
#include "menu_defn.h"
#include "pad.h"
#include "patch.h"
#include "pref.h"
#include "savestate.h"
#include "scratch.h"
#include "tetris.h"
#include "timer.h"

#include <banans.h>
#include <cardio.h>
#include <dpad.h>
#include <freeze.h>
#include <cstring>
#include "sfx.h"
#include "version.h"
#include "modlink.h"

namespace main {

static void (*s_draw_debug_text_trampoline)();
static void (*s_process_inputs_trampoline)();
static u32 (*s_PADRead_tramp)(mkb::PADStatus* statuses);

static void perform_assembly_patches() {
    // Inject the run function at the start of the main game loop
    // Hooked after Workshop Mod's tick()
    patch::write_branch_bl(reinterpret_cast<void*>(0x80270704),
                           reinterpret_cast<void*>(start_main_loop_assembly));

    /* Remove OSReport call ``PERF : event is still open for CPU!``
    since it reports every frame, and thus clutters the console */
    // Only needs to be applied to the US version
    patch::write_nop(reinterpret_cast<void*>(0x80033E9C));

    // Nop the conditional that guards `draw_debugtext`, enabling it even when debug mode is
    // disabled
    patch::write_nop(reinterpret_cast<void*>(0x80299f54));

    // Titlescreen patches
    mkb::strcpy(reinterpret_cast<char*>(0x8047f4ec), "SMB2 PRACTICE MOD");
    patch::write_branch(reinterpret_cast<void*>(0x8032ad0c),
                        reinterpret_cast<void*>(main::custom_titlescreen_text_color));
}

static void unlock_everything() {
    // Don't yet know how to unlock the staff credits game from a fresh save...
    mkb::unlock_info.master_unlocked = true;
    mkb::unlock_info.monkeys = 99;
    mkb::unlock_info.staff_credits_game_unlocked = true;
    mkb::unlock_info.play_points = 99999;
    mkb::unlock_info.newest_play_point_record = 99999;
    mkb::unlock_info.movies = 0x0fff;
    mkb::unlock_info.party_games = 0x0001b600;
    mkb::unlock_info.g_movies_watched = 0x0fff;
    mkb::memset(mkb::cm_unlock_entries, 0xff, sizeof(mkb::cm_unlock_entries));
    mkb::memset(mkb::storymode_unlock_entries, 0xff, sizeof(mkb::storymode_unlock_entries));
}

void init() {
    mkb::OSReport("[pracmod] SMB2 Practice Mod v%d.%d.%d loaded\n",
                  version::PRACMOD_VERSION.major,
                  version::PRACMOD_VERSION.minor,
                  version::PRACMOD_VERSION.patch);

    perform_assembly_patches();

    heap::init();
    return;
    pref::init();
    cardio::init();
    draw::init();
    Tetris::get_instance().init();
    iw::init();
    savestate::init();
    timer::init();
    inputdisp::init();
    cmseg::init();
    freeze::init();
    sfx::init();
    menu_defn::init();
    scratch::init();

    s_draw_debug_text_trampoline = patch::hook_function(mkb::draw_debugtext, []() {
        // Drawing hook for UI elements.
        // Gets run at the start of smb2's function which draws debug text windows,
        // which is called at the end of smb2's function which draws the UI in general.

        draw::predraw();
        timer::disp();
        iw::disp();
        Tetris::get_instance().disp();
        scratch::disp();
        cmseg::disp();
        inputdisp::disp();
        menu_impl::disp();
        draw::disp();

        s_draw_debug_text_trampoline();
    });

    s_process_inputs_trampoline = patch::hook_function(mkb::process_inputs, []() {
        s_process_inputs_trampoline();

        // These run after all controller inputs have been processed on the current frame,
        // to ensure lowest input delay
        pad::tick();
        cardio::tick();
        unlock_everything();
        iw::tick();
        savestate::tick();
        menu_impl::tick();
        jump::tick();
        inputdisp::tick();
        gotostory::tick();
        cmseg::tick();
        banans::tick();
        scratch::tick();
    });

    s_PADRead_tramp = patch::hook_function(mkb::PADRead, [](mkb::PADStatus* statuses) {
        u32 ret = s_PADRead_tramp(statuses);

        // Dpad can modify effective stick input, shown by input display
        dpad::on_PADRead(statuses);
        inputdisp::on_PADRead(statuses);

        return ret;
    });
}

/*
 * This runs at the very start of the main game loop. Most per-frame code runs after
 * controller inputs have been read and processed however, to ensure the lowest input delay.
 */
void tick() {
    if (pref::get_debug_mode()) {
        mkb::dip_switches |= mkb::DIP_DEBUG | mkb::DIP_DISP;
    } else {
        mkb::dip_switches &= ~(mkb::DIP_DEBUG | mkb::DIP_DISP);
    }
    pad::on_frame_start();
    mkb::OSReport("[pracmod] DEBUG: tick()\n");
}

}  // namespace main
