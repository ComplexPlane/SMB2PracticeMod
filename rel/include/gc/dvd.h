#pragma once

#include <cstdint>
#include <cstddef>

namespace gc {

struct DVDFileInfo;
struct DVDDir;

extern "C" {

bool DVDOpen(char *fileName, DVDFileInfo *fileInfo);
int32_t DVDConvertPathToEntrynum(char *fileName);
bool DVDFastOpen(int32_t entrynum, DVDFileInfo *fileInfo);
void DVDChangeDir(char *dirName);
bool DVDOpenDir(char *dirName, DVDDir *dir);

// This used to be a documented public-facing SDK function but it was removed in post-2000 SDKs.
// Still present in SMB2 and potentially useful to us.
bool DVDConvertEntrynumToPath(int32_t entrynum, char *outPath, size_t maxPathSize);

}

}
