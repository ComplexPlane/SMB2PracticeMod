#include "mod.h"
#include "heap.h"
#include "patch.h"
#include "global.h"

#include <gc/os.h>
#include <mkb/mkb.h>

namespace mod {

Mod *gMod = nullptr;

void main() {
	// Create the heap to use with a size of 0x15000 bytes
	heap::makeHeap(0x15000);

	Mod *mod = new Mod();
	mod->init();
}

Mod::Mod() {
	
}

void Mod::init() {
	performAssemblyPatches();
	
	gMod = this;

	// global::unknownDrawFunc1_trampoline = patch::hookFunction(
	// 	mkb::unknownDrawFunc1, 
	// 	[](double param1, double param2, double param3, double param4)
	// {
	// 	gc::OSReport("Poopes\n");
	// 	if (global::unknownDrawFunc1Enabled)
	// 	{
	// 		global::unknownDrawFunc1_trampoline(param1, param2, param3, param4);
	// 		gc::OSReport("udf1 enabled:  %d %d %d %d\n", param1, param2, param3, param4);
	// 	} 
	// 	else
	// 	{
	// 		gc::OSReport("udf1 disabled: %d %d %d %d\n", param1, param2, param3, param4);
	// 	}
	// });
}

}