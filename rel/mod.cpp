#include "mod.h"
#include "heap.h"
#include "patch.h"
#include "global.h"
#include "pad.h"
#include "bigmem.h"

#include <gc/gc.h>
#include <mkb/mkb.h>

#include <cstdint>

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

//static const char *getSoundErrStrWithNewline()
//{
//  return "SOUND_%s_###_at_%s\n";
//}

void Mod::init() {
  performAssemblyPatches();

  gc::OSReport("[mod] ApeSphere loaded\n");

  gMod = this;

  // Nop the conditional that guards `drawDebugText`, enabling it even when debug mode is disabled
  patch::writeNop(reinterpret_cast<void *>(0x80299f54));

//  global::tetris.init();
//  bigmem::init();

  global::drawDebugText_trampoline = patch::hookFunction(
      mkb::drawDebugText,
      []() {
        // Drawing hook for UI elements.
        // Gets run at the start of smb2's function which draws debug text windows,
        // which is called at the end of smb2's function which draws the UI in general.

        // gc::OSReport("Before drawDebugText()\n");
//        global::tetris.update();

        global::drawDebugText_trampoline();
        // gc::OSReport("After drawDebugText()\n");
      });

//  global::DVDOpen_trampoline = patch::hookFunction(
//      gc::DVDOpen,
//      [](char *fileName, gc::DVDFileInfo *fileInfo) {
//        gc::OSReport("[mod] DVDOpen(\"%s\", ...)\n", fileName);
//        return global::DVDOpen_trampoline(fileName, fileInfo);
//      });
//
//  global::DVDFastOpen_trampoline = patch::hookFunction(
//      gc::DVDFastOpen,
//      [](int32_t entrynum, gc::DVDFileInfo *fileInfo) {
//        char entrynumPath[128];
//        gc::DVDConvertEntrynumToPath(entrynum, entrynumPath, sizeof(entrynumPath));
//        gc::OSReport("[mod] DVDFastOpen(0x%08X, ...) -> path = \"%s\"\n", entrynum, entrynumPath);
//
//        return global::DVDFastOpen_trampoline(entrynum, fileInfo);
//      });
//
//  global::DVDChangeDir_trampoline = patch::hookFunction(
//      gc::DVDChangeDir,
//      [](char *dirName) {
//        gc::OSReport("[mod] DVDChangeDir(\"%s\")\n", dirName);
//        global::DVDChangeDir_trampoline(dirName);
//      });
//
//  global::DVDOpenDir_trampoline = patch::hookFunction(
//      gc::DVDOpenDir,
//      [](char *dirName, gc::DVDDir *dir) {
//        gc::OSReport("[mod] DVDOpenDir(\"%s\", ...)\n", dirName);
//        return global::DVDOpenDir_trampoline(dirName, dir);
//      });
//
//  global::OSAllocFromHeap_trampoline = patch::hookFunction(
//    gc::OSAllocFromHeap,
//    [&](gc::OSHeapHandle heap, uint32_t size)
//  {
//    gc::OSReport("[mod] OSAllocFromHeap(0x%08X, %d)\n", heap, size);
//    return global::OSAllocFromHeap_trampoline(heap, size);
//  });

  global::OSCreateHeap_trampoline = patch::hookFunction(
      gc::OSCreateHeap,
      [](void *start, void *end) {
        gc::OSHeapHandle ret = global::OSCreateHeap_trampoline(start, end);
        gc::OSReport("[mod] OSCreateHeap(0x%08X, 0x%08X) -> %d (size 0x%08X)\n",
            start,
            end,
            ret,
            reinterpret_cast<uint32_t>(end) - reinterpret_cast<uint32_t>(start));
        return ret;
      });

  global::OSDestroyHeap_trampoline = patch::hookFunction(
      gc::OSDestroyHeap,
      [](gc::OSHeapHandle heap) {
        gc::OSReport("[mod] OSDestroyHeap(%d)\n", heap);
        global::OSDestroyHeap_trampoline(heap);
      });
//
//  global::OSFreeToHeap_trampoline = patch::hookFunction(
//      gc::OSFreeToHeap,
//      [](gc::OSHeapHandle heap, void *ptr) {
//        gc::OSReport("[mod] OSFreeToHeap(%d, 0x%08X)\n", heap, ptr);
//        global::OSFreeToHeap_trampoline(heap, ptr);
//      });
//
  global::OSInitAlloc_trampoline = patch::hookFunction(
      gc::OSInitAlloc,
      [](void *arenaStart, void *arenaEnd, int maxHeaps) {
        gc::OSReport("[mod] OSInitAlloc(0x%08X, 0x%08X, %d)\n", arenaStart, arenaEnd, maxHeaps);
        return global::OSInitAlloc_trampoline(arenaStart, arenaEnd, maxHeaps);
      });
//
//  global::OSSetCurrentHeap_trampoline = patch::hookFunction(
//      gc::OSSetCurrentHeap,
//      [](gc::OSHeapHandle heap) {
//        gc::OSReport("[mod] OSSetCurrentHeap(%d)\n", heap);
//        return global::OSSetCurrentHeap_trampoline(heap);
//      });

  // Move GXCopyDisp call to top of gxFinishFrame function
//  global::gxFinishFrame_trampoline = patch::hookFunction(
//      mkb::gxFinishFrame,
//      []() {
//        gc::GXCopyDisp(mkb::graphicsInfo->activeFramebuffer, gc::GX_TRUE);
//        global::gxFinishFrame_trampoline();
//      });
//  patch::writeNop(reinterpret_cast<void *>(0x800649f8));

//  patch::writeBranch(reinterpret_cast<void *>(0x80064cd0), reinterpret_cast<void *>(0x80064cf0));
//  patch::writeBranchBL(reinterpret_cast<void *>(0x80064cf4), reinterpret_cast<void *>(getLockedCache));

  global::OSSetArenaLo_trampoline = patch::hookFunction(
      gc::OSSetArenaLo,
      [](void *newLo) {
        void *oldLo = gc::OSGetArenaLo();
        gc::OSReport("[mod] OSSetArenaLo(0x%08X) (old: 0x%08X)\n", newLo, oldLo);
        global::OSSetArenaLo_trampoline(newLo);
      });

  global::OSSetArenaHi_trampoline = patch::hookFunction(
      gc::OSSetArenaHi,
      [](void *newHi) {
        void *oldHi = gc::OSGetArenaHi();
        gc::OSReport("[mod] OSSetArenaHi(0x%08X) (old: 0x%08X)\n", newHi, oldHi);
        global::OSSetArenaHi_trampoline(newHi);
      });

  global::OSAllocFromArenaLo_trampoline = patch::hookFunction(
      gc::OSAllocFromArenaLo,
      [](uint32_t size, uint32_t align) {
        void *ret = global::OSAllocFromArenaLo_trampoline(size, align);
        gc::OSReport("[mod] OSAllocFromArenaLo(0x%08X, 0x%08X) -> 0x%08X\n", size, align, ret);
        return ret;
      });

  global::OSAllocFromArenaHi_trampoline = patch::hookFunction(
      gc::OSAllocFromArenaHi,
      [](uint32_t size, uint32_t align) {
        void *ret = global::OSAllocFromArenaHi_trampoline(size, align);
        gc::OSReport("[mod] OSAllocFromArenaHi(0x%08X, 0x%08X) -> 0x%08X\n", size, align, ret);
        return ret;
      });

//  global::createGameHeaps_trampoline = patch::hookFunction(
//      mkb::createGameHeaps,
//      [](int param1) {
//        if (param1 == 0)
//        {
//          gc::OSReport("[mod] Begin createGameHeaps bigmem override\n");
//          bigmem::createGameHeapsUsingExtraMem();
//          gc::OSReport("[mod] End createGameHeaps bigmem override\n");
//        }
//        else
//        {
//          gc::OSReport("[mod] Begin createGameHeaps(0x%08X)\n", param1);
//          global::createGameHeaps_trampoline(param1);
//          gc::OSReport("[mod] End createGameHeaps(0x%08X)\n", param1);
//        }
//      });

}

}
