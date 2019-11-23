#pragma once

#include <cstdint>

namespace gc::OSModule {

struct OSModuleInfo
{
	uint32_t id;
	OSModuleInfo *next;
	OSModuleInfo *prev;
	uint32_t numSections;
	uint32_t sectionInfoOffset;
	uint32_t nameOffset;
	uint32_t nameSize;
	uint32_t version;
} __attribute__((__packed__));

extern "C" {

bool OSLink(OSModuleInfo *newModule, void *bss);
bool OSUnlink(OSModuleInfo *oldModule);

}

}