#include "mod.h"
#include "patch.h"
#include "assembly.h"
#include "heap.h"
#include "pad.h"
#include "global.h"

#include <gc/os.h>

#include <cinttypes>

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

void enableDebugMode()
{
#ifdef MKB2_US
	uint32_t Offset = 0x6FB90;
#elif defined MKB2_JP
	uint32_t Offset = 0x29898;
#elif defined MKB2_EU
	uint32_t Offset = 0x29938;
#endif
	
	/* Should check to see if this value ever gets cleared. 
		If not, then the value should only be set once */
	*reinterpret_cast<uint32_t *>(reinterpret_cast<uint32_t>(
		heap::HeapData.MainLoopBSSLocation) + Offset) |= 
		((1 << 0) | (1 << 1)); // Turn on the 0 and 1 bits
}

void run()
{
	// Make sure there are no issues with the heap
	// heap::checkHeap();

	// Make sure debug mode is enabled
	enableDebugMode();

	if (pad::buttonChordPressed(pad::PAD_BUTTON_L, pad::PAD_BUTTON_X)) {
		gc::OSReport("Draw function toggled\n");
		global::unknownDrawFunc1Enabled = !global::unknownDrawFunc1Enabled;
	}
}

}
