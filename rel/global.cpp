#include "global.h"

// Idk how to best organize and structure data (let alone whether a global module like this is a
// good idea) like this but let's just mess around like this for now

namespace global {

void (*unknownDrawFunc1_trampoline)(double param1, double param2, double param3, double param4) = nullptr;
bool unknownDrawFunc1Enabled = true;

}