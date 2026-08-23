#include "systems/main.h"

#include "systems/assembly.h"
#include "systems/binds.h"
#include "systems/cardio.h"
#include "systems/heap.h"
#include "systems/menu_defn.h"
#include "systems/menu_impl.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "systems/savest.h"
#include "systems/version.h"
#include "utils/base.h"
#include "utils/draw.h"
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
#include "mods/timer.h"
#include "mods/unlock.h"
#include "mods/validate.h"

static void perform_assembly_patches() {
    // Inject the run function at the start of the main game loop
    // Hooked after Workshop Mod's tick()
    Patch_WriteBranchBL(Rel_RelocateAddr(0x80270704), (void *)(asm_start_main_loop_assembly));

    /* Remove OSReport call ``PERF : event is still open for CPU!``
    since it reports every frame, and thus clutters the console */
    // Only needs to be applied to the US version
    Patch_WriteNop(Rel_RelocateAddr(0x80033E9C));

    // Nop the conditional that guards `draw_debugtext`, enabling it even when debug mode is
    // disabled
    // Note that this hook is stil necessary even though we're using `textdraw_...()` for drawing
    // text
    Patch_WriteNop(Rel_RelocateAddr(0x80299f54));
    // Nop this pausemenu screenshot call so we can call it when we want to
    Patch_WriteNop(Rel_RelocateAddr(0x80270aac));

    // Titlescreen patches
    mkb_strcpy((char *)Rel_RelocateAddr(0x8047f4ec), "SMB2 PRACTICE MOD");
    Patch_WriteBranch(Rel_RelocateAddr(0x8032ad0c),
                       (void *)(asm_custom_titlescreen_text_color));
}

static u32 pad_read_hook(mkb_PADStatus *statuses);

TRAMP(s_pad_read_tramp, mkb_PADRead, pad_read_hook);

static u32 pad_read_hook(mkb_PADStatus *statuses) {
    u32 ret = s_pad_read_tramp.chain(statuses);

    // Dpad can modify effective stick input, shown by input display
    Dpad_OnPADRead(statuses);
    // pad collects original inputs before they are modified by the game
    Pad_OnPADRead(statuses);

    return ret;
}

static void process_inputs_hook();

TRAMP(s_process_inputs_tramp, mkb_process_inputs, process_inputs_hook);

static void process_inputs_hook() {
    s_process_inputs_tramp.chain();

    // These run after all controller inputs have been processed on the current frame,
    // to ensure lowest input delay
    Pad_Tick();
    Binds_Tick();
    Card_Tick();
    Unlock_Tick();
    IW_Tick();
    SS_Tick();
    SSUI_Tick();
    MenuImpl_Tick();  // anything checking for pref changes should run after MenuImpl_Tick()
    Fallout_Tick();
    Jump_Tick();     // (edits physics preset)
    Physics_Tick();  // anything editing physics presets must run before Physics_Tick()
    InputDisp_Tick();
    GoToStory_Tick();
    CM_Tick();
    Banans_Tick();
    Marathon_Tick();
    BallColor_Tick();
    Freecam_Tick();
    ILBattle_Tick();
    ILMark_Tick();
    Camera_Tick();
    StageEdits_Tick();
    Validate_Tick();
    Scratch_Tick();
    // Pref runs last to track the prefs from the previous frame
    Pref_Tick();
}

static void draw_debug_text_hook();

TRAMP(s_draw_debug_text_tramp, mkb_draw_debugtext, draw_debug_text_hook);

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

    Draw_PreDraw();
    Timer_Disp();
    IW_Disp();
    ILBattle_Disp();
    CM_Disp();
    InputDisp_Disp();
    MenuImpl_Disp();
    Draw_Disp();
    ILMark_Disp();
    Physics_Disp();
    Scratch_Disp();
}

static void smd_game_ready_init_hook();

TRAMP(s_smd_game_ready_init_tramp, mkb_smd_game_ready_init, smd_game_ready_init_hook);

static void smd_game_ready_init_hook() {
    StageEdits_SmdGameReadyInit();
    BallColor_SwitchMonkey();
    s_smd_game_ready_init_tramp.chain();
}

static void smd_game_play_tick_hook();

TRAMP(s_smd_game_play_tick_tramp, mkb_smd_game_play_tick, smd_game_play_tick_hook);

static void smd_game_play_tick_hook() {
    s_smd_game_play_tick_tramp.chain();
    Validate_Run();
    ILMark_ValidateAttempt();
    ILBattle_ValidateAttempt();
}

// Hook for mkb_load_additional_rel
static bool os_link_hook(mkb_OSModuleHeader *rel_buffer, void *bss_buffer);

TRAMP(s_os_link_tramp, mkb_OSLink, os_link_hook);

static bool os_link_hook(mkb_OSModuleHeader *rel_buffer, void *bss_buffer) {
    bool ret = s_os_link_tramp.chain(rel_buffer, bss_buffer);

    // Main game init functions
    if ((RelId)rel_buffer->info.id == Rel_MainGame) {
        HOOK_TRAMP(s_smd_game_ready_init_tramp);
        HOOK_TRAMP(s_smd_game_play_tick_tramp);
        Jump_PatchMinimap();
    }
    // Sel_ngc init functions
    // else if ((RelId)rel_buffer->info.id == Rel_SelNgc) {
    // }

    return ret;
}

void Main_Init() {
    Version_Init();

    mkb_OSReport("[pracmod] SMB2 Practice Mod v%s loaded\n", Version_GetStr());

    perform_assembly_patches();

    Heap_Init();
    Card_Init();
    Pref_Init();
    Unlock_Init();
    Draw_Init();
    Physics_Init();
    IW_Init();
    SS_Init();
    Timer_Init();
    InputDisp_Init();
    CM_Init();
    BallColor_Init();
    Sfx_Init();
    MenuDefn_Init();
    Freecam_Init();
    Hide_Init();
    ILMark_Init();
    Camera_Init();
    Fallout_Init();
    StageEdits_Init();
    Scratch_Init();
    Validate_Init();

    HOOK_TRAMP(s_pad_read_tramp);
    HOOK_TRAMP(s_process_inputs_tramp);
    HOOK_TRAMP(s_draw_debug_text_tramp);
    HOOK_TRAMP(s_os_link_tramp);
}

/*
 * This runs at the very start of the main game loop. Most per-frame code runs after
 * controller inputs have been read and processed however, to ensure the lowest input delay.
 */
void Main_Tick() {
    if (Pref_Get(Pref_DebugMode)) {
        mkb_dip_switches |= mkb_DIP_DEBUG | mkb_DIP_DISP;
    } else {
        mkb_dip_switches &= ~(mkb_DIP_DEBUG | mkb_DIP_DISP);
    }
    Pad_OnFrameStart();
}
