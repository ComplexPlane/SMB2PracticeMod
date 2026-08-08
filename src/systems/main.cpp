#include "mkb/mkb.h"

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
#include "utils/macro_utils.h"
#include "utils/patch.h"
#include "utils/relutil.h"
#include "utils/savest.h"

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
#include "mods/storytimer.h"
#include "mods/tetris.h"
#include "mods/timer.h"
#include "mods/unlock.h"

namespace main {

static void perform_assembly_patches() {
    // Inject the run function at the start of the main game loop
    // Hooked after Workshop Mod's tick()
    patch::write_branch_bl(relutil::relocate_addr(0x80270704),
                           reinterpret_cast<void*>(start_main_loop_assembly));

    /* Remove OSReport call ``PERF : event is still open for CPU!``
    since it reports every frame, and thus clutters the console */
    // Only needs to be applied to the US version
    patch::write_nop(relutil::relocate_addr(0x80033E9C));

    // Nop the conditional that guards `draw_debugtext`, enabling it even when debug mode is
    // disabled
    patch::write_nop(relutil::relocate_addr(0x80299f54));
    // Nop this pausemenu screenshot call so we can call it when we want to
    patch::write_nop(relutil::relocate_addr(0x80270aac));

    // Titlescreen patches
    mkb::strcpy(reinterpret_cast<char*>(relutil::relocate_addr(0x8047f4ec)), "SMB2 PRACTICE MOD");
    patch::write_branch(relutil::relocate_addr(0x8032ad0c),
                        reinterpret_cast<void*>(main::custom_titlescreen_text_color));
}

TRAMP(s_PADRead_tramp, mkb::PADRead, [](mkb::PADStatus* statuses) {
    u32 ret = s_PADRead_tramp.chain(statuses);

    // Dpad can modify effective stick input, shown by input display
    dpad::on_PADRead(statuses);
    inputdisp::on_PADRead(statuses);

    return ret;
});

TRAMP(s_process_inputs_tramp, mkb::process_inputs, []() {
    s_process_inputs_tramp.chain();

    // These run after all controller inputs have been processed on the current frame,
    // to ensure lowest input delay
    pad::tick();
    binds::tick();
    cardio::tick();
    unlock::tick();
    fallout::tick();
    physics::tick();
    iw::tick();
    // storytimer::tick();
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
    ilbattle::tick();
    ilmark::tick();
    camera::tick();
    stage_edits::tick();
    scratch::tick();
});

TRAMP(s_draw_debug_text_tramp, mkb::draw_debugtext, []() {
    // Drawing hook for UI elements.
    // Gets run at the start of smb2's function which draws debug text windows,
    // which is called at the end of smb2's function which draws the UI in general.

    s_draw_debug_text_tramp.chain();

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
    // storytimer::disp();
    Tetris::get_instance().disp();
    ilbattle::disp();
    cmseg::disp();
    inputdisp::disp();
    menu_impl::disp();
    draw::disp();
    ilmark::disp();
    scratch::disp();
});

// Hook for mkb::load_additional_rel
TRAMP(s_OSLink_tramp, mkb::OSLink, [](mkb::OSModuleHeader* rel_buffer, void* bss_buffer) {
    bool ret = s_OSLink_tramp.chain(rel_buffer, bss_buffer);

    // Main game init functions
    if (rel_buffer->info.id == relutil::ModuleId::MainGame) {
        stage_edits::main_game_init();
    }
    // Sel_ngc init functions
    // else if (rel_buffer->info.id == relutil::ModuleId::SelNgc) {
    // }

    return ret;
});

void init() {
    version::init();

    mkb::OSReport("[pracmod] SMB2 Practice Mod v%s loaded\n", version::get_version_str());

    perform_assembly_patches();
    mkb::OSReport("performed assembly patches\n");

    heap::init();
    cardio::init();
    pref::init();
    unlock::init();
    draw::init();
    Tetris::get_instance().init();
    physics::init();
    iw::init();
    savest::init();
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

    HOOK_TRAMP(s_PADRead_tramp);
    HOOK_TRAMP(s_process_inputs_tramp);
    HOOK_TRAMP(s_draw_debug_text_tramp);
    HOOK_TRAMP(s_OSLink_tramp);
}

/*
 * This runs at the very start of the main game loop. Most per-frame code runs after
 * controller inputs have been read and processed however, to ensure the lowest input delay.
 */
void tick() {
    if (pref::get(pref::BoolPref::DebugMode)) {
        mkb::dip_switches |= mkb::DIP_DEBUG | mkb::DIP_DISP;
    } else {
        mkb::dip_switches &= ~(mkb::DIP_DEBUG | mkb::DIP_DISP);
    }
    pad::on_frame_start();
}

}  // namespace main
