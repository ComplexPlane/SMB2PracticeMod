#include "mod.h"
#include "patch.h"
#include "assembly.h"
#include "heap.h"
#include "pad.h"
#include "global.h"

#include <gc/gc.h>
#include <mkb/mkb.h>

#include <cinttypes>
#include <cstring>

namespace mod {

bool Mod::performRelPatches(gc::OSModuleInfo *newModule, void *bss)
{
	// Call the original function immediately, as the REL file should be linked before applying patches
	const bool Result = mPFN_OSLink_trampoline(newModule, bss);
	
	// Make sure a REL file is currently loaded
	if (!Result)
	{
		return Result;
	}
	
	// uint32_t NewModuleRaw = reinterpret_cast<uint32_t>(newModule);
	switch (newModule->id)
	{
		default:
		{
			return Result;
		}
	}
}

void Mod::performAssemblyPatches()
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
#ifdef SMB2_US
	// Only needs to be applied to the US version
	patch::writeNop(reinterpret_cast<void *>(0x80033E9C));
#endif
}

static void resetGXFifos()
{
  static gc::GXFifoObj tmpFifoObj = {};

  auto *fifoBuf1 = reinterpret_cast<void *>(0x815d5b80);
  auto *fifoBuf2 = reinterpret_cast<void *>(0x814d5b80);
  constexpr uint32_t FIFO_BUF_SIZE = 0x100000;

  gc::GXFifoObj *gpFifoObj = gc::GXGetGPFifo();
  gc::GXFifoObj *cpuFifoObj = gc::GXGetCPUFifo();
  void *gpFifoBuf = nullptr; // The FIFO buffer of the GXFifoObj the GP is currently attached to
  void *cpuFifoBuf = nullptr; // The FIFO buffer of the GXFifoObj the CPU is currently attached to

  if (mkb::graphicsInfo->fifos[0] == gpFifoObj)
  {
    gpFifoBuf = fifoBuf1;
    cpuFifoBuf = fifoBuf2;
  }
  else
  {
    gpFifoBuf = fifoBuf2;
    cpuFifoBuf = fifoBuf1;
  }

  //
  // Reset the GP FIFO
  //

  // Wait until GP has finished reading all commands from currently attached FIFO
  gc::GXBool throwAway = gc::GX_FALSE, readIdle = gc::GX_FALSE;
  while (!readIdle)
  {
    gc::GXGetGPStatus(&throwAway, &throwAway, &readIdle, &throwAway, &throwAway);
  }

  // Point temporary fifo obj to gp fifo buffer
  tmpFifoObj = {};
  gc::GXInitFifoBase(&tmpFifoObj, gpFifoBuf, FIFO_BUF_SIZE);
  gc::GXInitFifoPtrs(&tmpFifoObj, gpFifoBuf, gpFifoBuf);

  // Attach temporary fifo obj to GP
  gc::GXSetGPFifo(&tmpFifoObj);

  // Reset the fifo obj the GPU was attached to, then reattach it
  *gpFifoObj = {};
  gc::GXInitFifoBase(gpFifoObj, gpFifoBuf, FIFO_BUF_SIZE);
  gc::GXInitFifoPtrs(gpFifoObj, gpFifoBuf, gpFifoBuf);
  gc::GXSetGPFifo(gpFifoObj);

  //
  // Reset the CPU FIFO
  //

  gc::GXSaveCPUFifo(cpuFifoObj); // Make sure there aren't any cached pending commands that need to be flushed to the CPU FIFO buffer

  tmpFifoObj = {};
  gc::GXInitFifoBase(&tmpFifoObj, cpuFifoBuf, FIFO_BUF_SIZE);
  gc::GXInitFifoPtrs(&tmpFifoObj, cpuFifoBuf, cpuFifoBuf);
  gc::GXSetCPUFifo(&tmpFifoObj);

  *cpuFifoObj = {};
  gc::GXInitFifoBase(cpuFifoObj, cpuFifoBuf, FIFO_BUF_SIZE);
  gc::GXInitFifoPtrs(cpuFifoObj, cpuFifoBuf, cpuFifoBuf);
  gc::GXSetCPUFifo(cpuFifoObj);
}

static uint8_t magicRegion1[224];
static uint8_t magicRegion2[112];
static uint8_t magicRegion3[28];
static uint16_t mkbTimer;

void run()
{
  // Enable debug mode (appears to need to be called every frame)
  mkb::dipSwitches |= mkb::DIP_DEBUG | mkb::DIP_DISP;

//  if (pad::buttonPressed(pad::PAD_BUTTON_Z))
//  {
//    memcpy(magicRegion1, reinterpret_cast<void *>(0x8054E03C), sizeof(magicRegion1));
//    memcpy(magicRegion2, reinterpret_cast<void *>(0x805BC974), sizeof(magicRegion2));
//    memcpy(magicRegion3, reinterpret_cast<void *>(0x805BD82E), sizeof(magicRegion3));
//    mkbTimer = *reinterpret_cast<uint16_t *>(0x80553974);
//
////    bool enable = gc::OSDisableInterrupts();
//
//    memcpy(global::lockedCacheSave, reinterpret_cast<void *>(0xE0000000), sizeof(global::lockedCacheSave));
//
////    gc::OSRestoreInterrupts(enable);
//
////    while (true);
//  }
//
//  if (pad::buttonPressed(pad::PAD_BUTTON_X))
//  {
//    memcpy(reinterpret_cast<void *>(0x8054E03C), magicRegion1, sizeof(magicRegion1));
//    memcpy(reinterpret_cast<void *>(0x805BC974), magicRegion2, sizeof(magicRegion2));
//    memcpy(reinterpret_cast<void *>(0x805BD82E), magicRegion3, sizeof(magicRegion3));
//    *reinterpret_cast<uint16_t *>(0x80553974) = mkbTimer;

//    bool enable = gc::OSDisableInterrupts();
//
//    memcpy(reinterpret_cast<void *>(0xE0000000), global::lockedCacheSave, sizeof(global::lockedCacheSave));
//    memset(reinterpret_cast<void *>(0xE00001E0), 0xff, 0xca0); // Reset previous gx settings
//
//    gc::OSRestoreInterrupts(enable);

//    resetGXFifos();

//    while (true);
//  }
}

}
