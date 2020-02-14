#pragma once

#include <gc/os.h>

#include <cstdint>

namespace mod {

class Mod
{
public:
	Mod();
	void init();

private:
	bool performRelPatches(gc::OSModuleInfo *, void *);
	void performAssemblyPatches();
	
private:
	bool (*mPFN_OSLink_trampoline)(gc::OSModuleInfo *, void *) = nullptr;
};

}