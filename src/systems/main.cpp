#include "mkb/mkb.h"

#include "mods/validate.h"
#include "systems/assembly.h"
#include "systems/binds.h"
#include "systems/cardio.h"
#include "systems/goal.h"
#include "systems/heap.h"
#include "systems/menu_defn.h"
#include "systems/menu_impl.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "systems/savest.h"
#include "systems/textinfo.h"
#include "systems/version.h"
#include "utils/draw.h"
#include "utils/patch.h"
#include "utils/relutil.h"

#include "mods/ballcolor.h"
#include "mods/banans.h"
#include "mods/camera.h"
#include "mods/cmseg.h"
#include "mods/deathcounter.h"
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
#include "mods/menu_accel.h"
#include "mods/physics.h"
#include "mods/savest_ui.h"
#include "mods/scratch.h"
#include "mods/sfx.h"
#include "mods/stage_edits.h"
#include "mods/storyreset.h"
#include "mods/storytimer.h"
#include "mods/timer.h"
#include "mods/unlock.h"

namespace main {

static void perform_assembly_patches() {
    // Inject the run function at the start of the main game loop
    // Hooked after Workshop Mod's tick()
    patch::write_branch_bl(relutil::relocate_addr(0x80270704),
                           reinterpret_cast<void *>(start_main_loop_assembly));

    /* Remove OSReport call ``PERF : event is still open for CPU!``
    since it reports every frame, and thus clutters the console */
    // Only needs to be applied to the US version
    patch::write_nop(relutil::relocate_addr(0x80033E9C));

    // Nop the conditional that guards `draw_debugtext`, enabling it even when debug mode is
    // disabled
    // Note that this hook is stil necessary even though we're using `textdraw_...()` for drawing
    // text
    patch::write_nop(relutil::relocate_addr(0x80299f54));
    // Nop this pausemenu screenshot call so we can call it when we want to
    patch::write_nop(relutil::relocate_addr(0x80270aac));

    // Titlescreen patches
    mkb::strcpy(reinterpret_cast<char *>(relutil::relocate_addr(0x8047f4ec)), "SMB2 PRACTICE MOD");
    patch::write_branch(relutil::relocate_addr(0x8032ad0c),
                        reinterpret_cast<void *>(main::custom_titlescreen_text_color));
}

static patch::Tramp<mkb::PADRead> s_PADRead_tramp([](mkb::PADStatus *statuses) {
    u32 ret = s_PADRead_tramp.chain(statuses);

    // Dpad can modify effective stick input, shown by input display
    dpad::on_PADRead(statuses);
    // pad collects original inputs before they are modified by the game
    pad::on_PADRead(statuses);

    return ret;
});

static patch::Tramp<mkb::process_inputs> s_process_inputs_tramp([]() {
    s_process_inputs_tramp.chain();

    // These run after all controller inputs have been processed on the current frame,
    // to ensure lowest input delay
    pad::tick();
    binds::tick();
    cardio::tick();
    unlock::tick();
    iw::tick();
    savest::tick();
    savest_ui::tick();
    menu_impl::tick();  // anything checking for pref changes should run after menu_impl::tick()
    storyreset::tick();
    deathcounter::tick();
    storytimer::tick();
    fallout::tick();
    jump::tick();     // (edits physics preset)
    physics::tick();  // anything editing physics presets must run before physics::tick()
    inputdisp::tick();
    gotostory::tick();
    cmseg::tick();
    menu_accel::tick();
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

static patch::Tramp<mkb::draw_debugtext> s_draw_debug_text_tramp([]() {
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
    // timer::disp();
    iw::disp();
    deathcounter::disp();
    storytimer::disp();
    ilbattle::disp();
    cmseg::disp();
    timer::disp();
    inputdisp::disp();
    menu_impl::disp();
    draw::disp();
    ilmark::disp();
    physics::disp();
    scratch::disp();
    textinfo::disp();
});

static patch::Tramp<mkb::smd_game_ready_init> s_smd_game_ready_init_tramp([]() {
    stage_edits::smd_game_ready_init();
    ballcolor::switch_monkey();
    s_smd_game_ready_init_tramp.chain();
});

static patch::Tramp<mkb::smd_game_play_tick> s_smd_game_play_tick_tramp([]() {
    s_smd_game_play_tick_tramp.chain();
    validate::validate_run();
    ilmark::validate_attempt();
    ilbattle::validate_attempt();
});

// Hook for mkb::load_additional_rel
static patch::Tramp<mkb::OSLink> s_OSLink_tramp([](mkb::OSModuleHeader *rel_buffer,
                                                   void *bss_buffer) {
    bool ret = s_OSLink_tramp.chain(rel_buffer, bss_buffer);

    // Main game init functions
    if (static_cast<relutil::ModuleId::Type>(rel_buffer->info.id) == relutil::ModuleId::MainGame) {
        s_smd_game_ready_init_tramp.hook();
        s_smd_game_play_tick_tramp.hook();
        jump::patch_minimap();
        storytimer::init_main_game();
    }
    // Sel_ngc init functions
    // else if (relutil::ModuleId(rel_buffer->info.id) == relutil::ModuleId::SelNgc) {
    // }

    return ret;
});

void init() {
    version::init();

    mkb::OSReport("[pracmod] SMB2 Practice Mod v%s loaded\n", version::get_version_str());

    perform_assembly_patches();

    heap::init();
    cardio::init();
    pref::init();
    unlock::init();
    draw::init();
    physics::init();
    iw::init();
    savest::init();
    goal::init();
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

    s_PADRead_tramp.hook();
    s_process_inputs_tramp.hook();
    s_draw_debug_text_tramp.hook();
    s_OSLink_tramp.hook();
}

/*
 * This runs at the very start of the main game loop. Most per-frame code runs after
 * controller inputs have been read and processed however, to ensure the lowest input delay.
 */
void tick() {
    if (pref::get(pref::Pref::DebugMode)) {
        mkb::dip_switches |= mkb::DIP_DEBUG | mkb::DIP_DISP;
    } else {
        mkb::dip_switches &= ~(mkb::DIP_DEBUG | mkb::DIP_DISP);
    }
    pad::on_frame_start();
}

}  // namespace main
