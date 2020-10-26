#pragma once

#include <gc/mathtypes.h>

namespace gc
{

struct DVDFileInfo;
struct DVDDir;

extern "C" {

bool DVDOpen(char *fileName, DVDFileInfo *fileInfo);
s32 DVDConvertPathToEntrynum(char *fileName);
bool DVDFastOpen(s32 entrynum, DVDFileInfo *fileInfo);
void DVDChangeDir(char *dirName);
bool DVDOpenDir(char *dirName, DVDDir *dir);

// This used to be a documented public-facing SDK function but it was removed in post-2000 SDKs.
// Still present in SMB2 and potentially useful to us.
bool DVDConvertEntrynumToPath(s32 entrynum, char *outPath, u32 maxPathSize);

}

}
