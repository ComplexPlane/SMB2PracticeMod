#include "patch.h"
#include "assembly.h"
#include "heap.h"
#include "savestate.h"
#include "tetris.h"
#include "draw.h"
#include "timer.h"
#include "iw.h"
#include "pad.h"
#include "menu.h"

#include <mkb/mkb.h>

#include <cstring>

namespace main
{

static void (*s_drawDebugText_trampoline)();

static void performAssemblyPatches()
{
#ifdef MKB2_US
    uint32_t Offset = 0x600;
#elif defined MKB2_JP
    uint32_t Offset = 0x604;
#elif defined MKB2_EU
    uint32_t Offset = 0x604;
#endif
    // Inject the run function at the start of the main game loop
    patch::writeBranchBL(reinterpret_cast<void *>(reinterpret_cast<uint32_t>(
                                                      heap::HeapData.MainLoopRelLocation) + Offset),
                         reinterpret_cast<void *>(StartMainLoopAssembly));

    /* Remove OSReport call ``PERF : event is still open for CPU!``
    since it reports every frame, and thus clutters the console */
#ifdef MKB2_US
    // Only needs to be applied to the US version
    patch::writeNop(reinterpret_cast<void *>(0x80033E9C));
#endif

    // Nop the conditional that guards `draw_debugtext`, enabling it even when debug mode is disabled
    patch::writeNop(reinterpret_cast<void *>(0x80299f54));

    // IW-related patches
    patch::writeBranch(reinterpret_cast<void *>(0x80274804), reinterpret_cast<void *>(StageSelectMenuHook));
    patch::writeBranch(reinterpret_cast<void *>(0x8032a86c), reinterpret_cast<void *>(PauseMenuTextHook));

    // Titlescreen patches
    strcpy(reinterpret_cast<char *>(0x8047f4ec), "APESPHERE PRACTICE MOD");
    patch::writeBranch(reinterpret_cast<void *>(0x8032ad0c),
                       reinterpret_cast<void *>(main::CustomTitleScreenTextColor));
}

void init()
{
    gc::OSReport("[mod] ApeSphere loaded\n");

    performAssemblyPatches();

    heap::init();
    draw::init();
    Tetris::getInstance().init();
    savestate::init();
    timer::init();
    iw::init();

    s_drawDebugText_trampoline = patch::hookFunction(
        mkb::draw_debugtext, []()
        {
            // Drawing hook for UI elements.
            // Gets run at the start of smb2's function which draws debug text windows,
            // which is called at the end of smb2's function which draws the UI in general.

            draw::preDraw();
            draw::disp();
            timer::disp();
            iw::disp();
            Tetris::getInstance().disp();
            menu::disp();

            s_drawDebugText_trampoline();
        });
}

static void unlockEverything()
{
    // Don't yet know how to unlock the staff credits game from a fresh save...
    mkb::unlock_info.masterUnlocked = true;
    mkb::unlock_info.monkeys = 99;
    mkb::unlock_info.staffCreditsGameUnlocked = true;
    mkb::unlock_info.playPoints = 99999;
    mkb::unlock_info.newestPlayPointRecord = 99999;
    mkb::unlock_info.movies = 0x0fff;
    mkb::unlock_info.partyGames = 0x0001b600;
    mkb::unlock_info.moviesWatched = 0x0fff;
    memset(mkb::cm_unlock_entries, 0xff, sizeof(mkb::cm_unlock_entries));
    memset(mkb::storymode_unlock_entries, 0xff, sizeof(mkb::storymode_unlock_entries));
}

void tick()
{
    // Enable debug mode (appears to need to be called every frame)
//    mkb::dipSwitches |= mkb::DIP_DEBUG | mkb::DIP_DISP;

    unlockEverything();
    pad::tick();
    timer::tick();
    iw::tick();
    savestate::tick();
    menu::tick();
}

}
