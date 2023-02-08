#include "mkb/mkb.h"

#include "systems/assembly.h"
#include "systems/cardio.h"
#include "systems/heap.h"
#include "systems/menu_defn.h"
#include "systems/menu_impl.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "systems/version.h"
#include "utils/draw.h"
#include "utils/libsavest.h"
#include "utils/patch.h"

#include "mods/ballcolor.h"
#include "mods/banans.h"
#include "mods/cmseg.h"
#include "mods/dpad.h"
#include "mods/freecam.h"
#include "mods/freeze.h"
#include "mods/gotostory.h"
#include "mods/ilbattle.h"
#include "mods/inputdisp.h"
#include "mods/iw.h"
#include "mods/jump.h"
#include "mods/marathon.h"
#include "mods/moon.h"
#include "mods/savest_ui.h"
#include "mods/scratch.h"
#include "mods/sfx.h"
#include "mods/tetris.h"
#include "mods/timer.h"

namespace main {

static patch::Tramp<decltype(&mkb::draw_debugtext)> s_draw_debug_text_tramp;
static patch::Tramp<decltype(&mkb::process_inputs)> s_process_inputs_tramp;
static patch::Tramp<decltype(&mkb::PADRead)> s_PADRead_tramp;

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
    // Nop this pausemenu screenshot call so we can call it when we want to
    patch::write_nop(reinterpret_cast<void*>(0x80270aac));

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
    mkb::OSReport("[pracmod] SMB2 Practice Mod v%d.%d.%d loaded\n", version::PRACMOD_VERSION.major,
                  version::PRACMOD_VERSION.minor, version::PRACMOD_VERSION.patch);

    perform_assembly_patches();

    heap::init();
    pref::init();
    cardio::init();
    draw::init();
    Tetris::get_instance().init();
    iw::init();
    libsavest::init();
    timer::init();
    inputdisp::init();
    cmseg::init();
    freeze::init();
    sfx::init();
    menu_defn::init();
    freecam::init();
    scratch::init();

    patch::hook_function(s_draw_debug_text_tramp, mkb::draw_debugtext, []() {
        // Drawing hook for UI elements.
        // Gets run at the start of smb2's function which draws debug text windows,
        // which is called at the end of smb2's function which draws the UI in general.

        s_draw_debug_text_tramp.dest();

        // When the game is paused, screenshot the game's draw buffer before we draw our custom UI
        // elements. The original screenshot call is nopped.
        if (mkb::g_pause_status == 1) {
            mkb::take_pausemenu_screenshot(&mkb::fullscreen_texture_buf, 0, 0,
                                           mkb::current_render_mode->fbWidth,
                                           mkb::current_render_mode->efbHeight, mkb::GX_TF_RGB5A3);
        }

        draw::predraw();
        timer::disp();
        iw::disp();
        Tetris::get_instance().disp();
        ilbattle::disp();
        cmseg::disp();
        inputdisp::disp();
        menu_impl::disp();
        draw::disp();
        scratch::disp();
    });

    patch::hook_function(s_process_inputs_tramp, mkb::process_inputs, []() {
        s_process_inputs_tramp.dest();

        // These run after all controller inputs have been processed on the current frame,
        // to ensure lowest input delay
        pad::tick();
        cardio::tick();
        unlock_everything();
        iw::tick();
        savest_ui::tick();
        menu_impl::tick();
        jump::tick();
        inputdisp::tick();
        gotostory::tick();
        cmseg::tick();
        banans::tick();
        marathon::tick();
        ballcolor::tick();
        freecam::tick();
        moon::tick();
        ilbattle::tick();
        scratch::tick();
    });

    patch::hook_function(s_PADRead_tramp, mkb::PADRead, [](mkb::PADStatus* statuses) {
        u32 ret = s_PADRead_tramp.dest(statuses);

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
}

}  // namespace main
