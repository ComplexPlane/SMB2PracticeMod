#pragma once

#include "patch.h"
#include "tetris.h"
#include "pad.h"

#include <gc/dvd.h>

namespace mod::global {

extern void (*drawDebugText_trampoline)();

extern bool (*DVDOpen_trampoline)(char *fileName, gc::DVDFileInfo *fileInfo);
extern int32_t (*DVDConvertPathToEntrynum_trampoline)(char *filename);
extern bool (*DVDFastOpen_trampoline)(int32_t entrynum, gc::DVDFileInfo *fileInfo);
extern void (*DVDChangeDir_trampoline)(char *dirName);
extern bool (*DVDOpenDir_trampoline)(char *dirName, gc::DVDDir *dir);

extern Tetris tetris;

}