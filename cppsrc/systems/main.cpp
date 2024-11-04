#include "mkb/mkb.h"

#include "mods/validate.h"
#include "systems/assembly.h"
#include "systems/binds.h"
#include "systems/cardio.h"
#include "systems/heap.h"
#include "systems/menu_defn.h"
#include "systems/menu_impl.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "systems/version.h"
#include "utils/draw.h"
#include "utils/libsavest.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"
#include "utils/relutil.h"

#include "mods/ballcolor.h"
#include "mods/banans.h"
#include "mods/camera.h"
#include "mods/cmseg.h"
#include "mods/dpad.h"
#include "mods/fallout.h"
#include "mods/freecam.h"
#include "mods/gotostory.h"
#include "mods/hide.h"
#include "mods/ilbattle.h"
#include "mods/ilmark.h"
#include "mods/inputdisp.h"
#include "mods/iw.h"
#include "mods/jump.h"
#include "mods/marathon.h"
#include "mods/physics.h"
#include "mods/savest_ui.h"
#include "mods/scratch.h"
#include "mods/sfx.h"
#include "mods/stage_edits.h"
#include "mods/tetris.h"
#include "mods/timer.h"
#include "mods/unlock.h"

namespace main {

static patch::Tramp<decltype(&mkb::draw_debugtext)> s_draw_debug_text_tramp;
static patch::Tramp<decltype(&mkb::process_inputs)> s_process_inputs_tramp;
static patch::Tramp<decltype(&mkb::PADRead)> s_PADRead_tramp;
static patch::Tramp<decltype(&mkb::OSLink)> s_OSLink_tramp;
static patch::Tramp<decltype(&mkb::smd_game_ready_init)> s_smd_game_ready_init_tramp;
static patch::Tramp<decltype(&mkb::smd_game_play_tick)> s_smd_game_play_tick_tramp;

/*
 * This runs at the very start of the main game loop. Most per-frame code runs after
 * controller inputs have been read and processed however, to ensure the lowest input delay.
 */
static void tick() {
    // Replace overwritten function call
    mkb::perf_init_timer(4);

    if (pref::get(pref::BoolPref::DebugMode)) {
        mkb::dip_switches |= mkb::DIP_DEBUG | mkb::DIP_DISP;
    } else {
        mkb::dip_switches &= ~(mkb::DIP_DEBUG | mkb::DIP_DISP);
    }
    pad::on_frame_start();
}

static void perform_assembly_patches() {
    patch::write_branch_bl(reinterpret_cast<void*>(0x80270718), reinterpret_cast<void*>(tick));

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

void init() {
    version::init();

    mkb::OSReport("[pracmod] SMB2 Practice Mod v%s loaded\n", version::get_version_str());

    perform_assembly_patches();

    heap::init();
    cardio::init();
    pref::init();
    unlock::init();
    draw::init();
    Tetris::get_instance().init();
    physics::init();
    iw::init();
    libsavest::init();
    timer::init();
    inputdisp::init();
    cmseg::init();
    ballcolor::init();
    sfx::init();
    menu_defn::init();
    freecam::init();
    hide::init();
    ilmark::init();
    camera::init();
    fallout::init();
    stage_edits::init();
    scratch::init();
    validate::init();

    patch::hook_function(s_PADRead_tramp, mkb::PADRead, [](mkb::PADStatus* statuses) {
        u32 ret = s_PADRead_tramp.dest(statuses);

        // Dpad can modify effective stick input, shown by input display
        dpad::on_PADRead(statuses);
        // pad collects original inputs before they are modified by the game
        pad::on_PADRead(statuses);

        return ret;
    });

    patch::hook_function(s_process_inputs_tramp, mkb::process_inputs, []() {
        s_process_inputs_tramp.dest();

        // These run after all controller inputs have been processed on the current frame,
    // to ensure lowest input delay
        pad::tick();
        binds::tick();
        cardio::tick();
        unlock::tick();
        iw::tick();
        savest_ui::tick();
        menu_impl::tick();  // anything checking for pref changes should run after menu_impl::tick()
        fallout::tick();
        jump::tick();     // (edits physics preset)
        physics::tick();  // anything editing physics presets must run before physics::tick()
        inputdisp::tick();
        gotostory::tick();
        cmseg::tick();
        banans::tick();
        marathon::tick();
        ballcolor::tick();
        freecam::tick();
        ilbattle::tick();
        ilmark::tick();
        camera::tick();
        stage_edits::tick();
        validate::tick();
        scratch::tick();
        // Pref runs last to track the prefs from the previous frame
        pref::tick();
    });

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
        ilmark::disp();
        physics::disp();
        scratch::disp();
    });

    // Hook for mkb::load_additional_rel
    patch::hook_function(
        s_OSLink_tramp, mkb::OSLink, [](mkb::OSModuleHeader* rel_buffer, void* bss_buffer) {
            bool ret = s_OSLink_tramp.dest(rel_buffer, bss_buffer);

            // Main game init functions
            if (relutil::ModuleId(rel_buffer->info.id) == relutil::ModuleId::MainGame) {
                patch::hook_function(s_smd_game_ready_init_tramp, mkb::smd_game_ready_init, []() {
                    stage_edits::smd_game_ready_init();
                    ballcolor::switch_monkey();
                    s_smd_game_ready_init_tramp.dest();
                });
                patch::hook_function(s_smd_game_play_tick_tramp, mkb::smd_game_play_tick, []() {
                    s_smd_game_play_tick_tramp.dest();
                    validate::validate_run();
                    ilmark::validate_attempt();
                    ilbattle::validate_attempt();
                });
                jump::patch_minimap();
            }
            // Sel_ngc init functions
            // else if (relutil::ModuleId(rel_buffer->info.id) == relutil::ModuleId::SelNgc) {
            // }

            return ret;
        });
}

}  // namespace main
