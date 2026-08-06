#include "main.h"

#include "utils/base.h"

#include "mkb/mkb2_ghidra.h"
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
#include "mods/physics.h"
#include "mods/savest_ui.h"
#include "mods/scratch.h"
#include "mods/sfx.h"
#include "mods/stage_edits.h"
#include "mods/storytimer.h"
#include "mods/tetris.h"
#include "mods/timer.h"
#include "mods/unlock.h"

static void draw_debug_text_hook();
static void process_inputs();
static u32 pad_read(mkb_PADStatus *statuses);
static bool os_link(mkb_OSModuleHeader *rel_buffer, void *bss_buffer);
static void ignore_sound_request(u32 g_sfx_idx);

TRAMP(s_draw_debug_text_tramp, mkb_draw_debugtext, draw_debug_text_hook);
TRAMP(s_process_inputs_tramp, mkb_process_inputs, process_inputs);
TRAMP(s_pad_read_tramp, mkb_PADRead, pad_read);
TRAMP(s_os_link_tramp, mkb_OSLink, os_link);
TRAMP(s_ignore_sound_request_tramp, mkb_call_SoundReqID_arg_0, ignore_sound_request);

static void perform_assembly_patches() {
    // Inject the run function at the start of the main game loop
    // Hooked after Workshop Mod's tick()
    patch_write_branch_bl((void *)(0x80270704), (void *)(asm_start_main_loop_assembly));

    /* Remove OSReport call ``PERF : event is still open for CPU!``
    since it reports every frame, and thus clutters the console */
    // Only needs to be applied to the US version
    patch_write_nop((void *)(0x80033E9C));

    // Nop the conditional that guards `draw_debugtext`, enabling it even when debug mode is
    // disabled
    patch_write_nop((void *)(0x80299f54));
    // Nop this pausemenu screenshot call so we can call it when we want to
    patch_write_nop((void *)(0x80270aac));

    // Titlescreen patches
    mkb_strcpy((char *)(0x8047f4ec), "SMB2 PRACTICE MOD");
    patch_write_branch((void *)(0x8032ad0c), (void *)(asm_custom_titlescreen_text_color));
}

static void process_inputs() {
    s_process_inputs_tramp.chain();

    // These run after all controller inputs have been processed on the current frame,
    // to ensure lowest input delay
    pad_tick();
    binds_tick();
    cardio_tick();
    unlock_tick();
    fallout_tick();
    physics_tick();
    iw_tick();
    storytimer_tick();
    deathcounter_tick();
    savest_ui_tick();
    menu_impl_tick();
    jump_tick();
    inputdisp_tick();
    gotostory_tick();
    cmseg_tick();
    banans_tick();
    marathon_tick();
    ballcolor_tick();
    freecam_tick();
    ilbattle_tick();
    ilmark_tick();
    camera_tick();
    stage_edits_tick();
    scratch_tick();
}

static void draw_debug_text_hook() {
    // Drawing hook for UI elements.
    // Gets run at the start of smb2's function which draws debug text windows,
    // which is called at the end of smb2's function which draws the UI in general.

    s_draw_debug_text_tramp.chain();

    // When the game is paused, screenshot the game's draw buffer before we draw our custom UI
    // elements. The original screenshot call is nopped.
    if (mkb_g_pause_status == 1) {
        mkb_take_pausemenu_screenshot(&mkb_fullscreen_texture_buf, 0, 0,
                                      mkb_current_render_mode->fbWidth,
                                      mkb_current_render_mode->efbHeight, mkb_GX_TF_RGB5A3);
    }

    draw_predraw();
    timer_disp();
    iw_disp();
    storytimer_disp();
    deathcounter_disp();
    tetris_disp();
    ilbattle_disp();
    cmseg_disp();
    inputdisp_disp();
    menu_impl_disp();
    draw_disp();
    ilmark_disp();
    scratch_disp();
}

static u32 pad_read(mkb_PADStatus *statuses) {
    u32 ret = s_pad_read_tramp.chain(statuses);

    // Dpad can modify effective stick input, shown by input display
    dpad_on_PADRead(statuses);
    inputdisp_on_PADRead(statuses);

    return ret;
}

static bool os_link(mkb_OSModuleHeader *rel_buffer, void *bss_buffer) {
    bool ret = s_os_link_tramp.chain(rel_buffer, bss_buffer);

    // Main game init functions
    if (rel_buffer->info.id == RelId_MainGame) {
        stage_edits_main_game_init();
    }
    // Sel_ngc init functions
    // else if (relutil::ModuleId(rel_buffer->info.id) == relutil::ModuleId::SelNgc) {
    // }

    return ret;
}

static void ignore_sound_request(u32 g_sfx_idx) {
}

void main_init() {
    version_init();

    mkb_OSReport("[pracmod] SMB2 Practice Mod v%s loaded\n", version_get_str());

    perform_assembly_patches();

    heap_init();
    cardio_init();
    pref_init();
    unlock_init();
    draw_init();
    tetris_init();
    physics_init();
    iw_init();
    savest_init();
    timer_init();
    inputdisp_init();
    cmseg_init();
    ballcolor_init();
    sfx_init();
    menu_init();
    freecam_init();
    hide_init();
    ilmark_init();
    camera_init();
    fallout_init();
    stage_edits_init();
    scratch_init();

    HOOK_TRAMP(s_pad_read_tramp);
    HOOK_TRAMP(s_process_inputs_tramp);
    HOOK_TRAMP(s_ignore_sound_request_tramp);
    HOOK_TRAMP(s_draw_debug_text_tramp);
    // Hook for mkb_load_additional_rel
    HOOK_TRAMP(s_os_link_tramp);
}

/*
 * This runs at the very start of the main game loop. Most per-frame code runs after
 * controller inputs have been read and processed however, to ensure the lowest input delay.
 */
void main_tick() {
    if (pref_get(Pref_DebugMode)) {
        mkb_dip_switches |= mkb_DIP_DEBUG | mkb_DIP_DISP;
    } else {
        mkb_dip_switches &= ~(mkb_DIP_DEBUG | mkb_DIP_DISP);
    }
    pad_on_frame_start();
}
