#include "patch.h"
#include "assembly.h"
#include "heap.h"
#include "savestate.h"
#include "tetris.h"
#include "draw.h"
#include "timer.h"
#include "iw.h"

#include <gc/gc.h>
#include <mkb/mkb.h>

#include <titlescreen.h>
#include <pad.h>

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

    // Nop the conditional that guards `drawDebugText`, enabling it even when debug mode is disabled
    patch::writeNop(reinterpret_cast<void *>(0x80299f54));
}

void init()
{
    gc::OSReport("[mod] ApeSphere loaded\n");

    performAssemblyPatches();

    heap::init();
    draw::init();
    Tetris::getInstance().init();
    savestate::init();
    titlescreen::init();
    timer::init();
    iw::init();

    s_drawDebugText_trampoline = patch::hookFunction(
        mkb::drawDebugText, []()
        {
            // Drawing hook for UI elements.
            // Gets run at the start of smb2's function which draws debug text windows,
            // which is called at the end of smb2's function which draws the UI in general.

            draw::preDraw();
            draw::disp();
            timer::disp();
            Tetris::getInstance().disp();

            s_drawDebugText_trampoline();
        });
}

void tick()
{
    // Enable debug mode (appears to need to be called every frame)
//    mkb::dipSwitches |= mkb::DIP_DEBUG | mkb::DIP_DISP;

    pad::tick();
    savestate::tick();
    timer::tick();
    iw::tick();
}

}
