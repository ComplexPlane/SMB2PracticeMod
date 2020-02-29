#include "mod.h"
#include "heap.h"
#include "patch.h"
#include "global.h"
#include "pad.h"

#include <gc/os.h>
#include <gc/dvd.h>
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

	global::DVDOpen_trampoline = patch::hookFunction(
		gc::DVDOpen,
		[](char *fileName, gc::DVDFileInfo *fileInfo)
	{
		gc::OSReport("DVDOpen(\"%s\", ...)\n", fileName);
		return global::DVDOpen_trampoline(fileName, fileInfo);
	});

	global::DVDConvertPathToEntrynum_trampoline = patch::hookFunction(
		gc::DVDConvertPathToEntrynum,
		[](char *fileName)
	{
		gc::OSReport("DVDConvertPathToEntrynum(\"%s\", ...)\n", fileName);
		return global::DVDConvertPathToEntrynum_trampoline(fileName);
	});

	global::DVDFastOpen_trampoline = patch::hookFunction(
		gc::DVDFastOpen,
		[](int32_t entrynum, gc::DVDFileInfo *fileInfo)
	{
		char entrynumPath[128];
		gc::DVDConvertEntrynumToPath(entrynum, entrynumPath, sizeof(entrynumPath));
		gc::OSReport("DVDFastOpen(0x%08x, ...) -> path = \"%s\"\n", entrynum, entrynumPath);

		return global::DVDFastOpen_trampoline(entrynum, fileInfo);
	});

	global::DVDChangeDir_trampoline = patch::hookFunction(
		gc::DVDChangeDir,
		[](char *dirName)
	{
		gc::OSReport("DVDChangeDir(\"%s\")\n", dirName);
		global::DVDChangeDir_trampoline(dirName);
	});

	global::DVDOpenDir_trampoline = patch::hookFunction(
		gc::DVDOpenDir,
		[](char *dirName, gc::DVDDir *dir)
	{
		gc::OSReport("DVDOpenDir(\"%s\", ...)\n", dirName);
		return global::DVDOpenDir_trampoline(dirName, dir);
	});
}

}