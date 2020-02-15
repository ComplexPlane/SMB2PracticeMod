#include "mod.h"
#include "heap.h"
#include "patch.h"
#include "global.h"
#include "pad.h"

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

	// Nop the conditional that guards `drawDebugText`, enabling it even when debug mode is disabled
	patch::writeNop(reinterpret_cast<void *>(0x80299f54));

	global::tetris.init();

	global::drawDebugText_trampoline = patch::hookFunction(
		mkb::drawDebugText, 
		[]()
	{
		// Drawing hook for UI elements.
		// Gets run at the start of smb2's function which draws debug text windows,
		// which is called at the end of smb2's function which draws the UI in general.

		// gc::OSReport("Before drawDebugText()\n");
		global::tetris.update();

		global::drawDebugText_trampoline();
		// gc::OSReport("After drawDebugText()\n");
	});
}

}