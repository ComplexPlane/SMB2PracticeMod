#include "mod.h"
#include "patch.h"
#include "assembly.h"
#include "heap.h"

#include <gc/OSModule.h>
#include <gc/OSAlloc.h>
#include <gc/OSError.h>

#include <cinttypes>

namespace mod {

bool Mod::performRelPatches(gc::OSModule::OSModuleInfo *newModule, void *bss)
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
	uint32_t *Address = reinterpret_cast<uint32_t *>(0x80033E9C);
	*Address = 0x60000000; // nop
	
	// Clear the cache for the address
	patch::clear_DC_IC_Cache(Address, sizeof(uint32_t));
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
	heap::checkHeap();
	
	// Make sure debug mode is enabled
	enableDebugMode();
}

}