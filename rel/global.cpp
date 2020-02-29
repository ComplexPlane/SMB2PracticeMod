#include "global.h"

// Idk how to best organize and structure data (let alone whether a global module like this is a
// good idea) like this but let's just mess around like this for now

namespace mod::global {

void (*drawDebugText_trampoline)() = nullptr;

bool (*DVDOpen_trampoline)(char *fileName, gc::DVDFileInfo *fileInfo) = nullptr;
int32_t (*DVDConvertPathToEntrynum_trampoline)(char *filename) = nullptr;
bool (*DVDFastOpen_trampoline)(int32_t entrynum, gc::DVDFileInfo *fileInfo) = nullptr;
void (*DVDChangeDir_trampoline)(char *dirName) = nullptr;
bool (*DVDOpenDir_trampoline)(char *dirName, gc::DVDDir *dir) = nullptr;

Tetris tetris;

}