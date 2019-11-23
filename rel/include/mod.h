#pragma once

#include <gc/OSModule.h>

#include <cstdint>

namespace mod {

class Mod
{
public:
	Mod();
	void init();

private:
	bool performRelPatches(gc::OSModule::OSModuleInfo *, void *);
	void performAssemblyPatches();
	
private:
	bool (*mPFN_OSLink_trampoline)(gc::OSModule::OSModuleInfo *, void *) = nullptr;
};

}