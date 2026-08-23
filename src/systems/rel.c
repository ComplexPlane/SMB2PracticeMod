#include "systems/main.h"

#include "mkb/mkb2_ghidra.h"

void _prolog() {
    // Run mod init function
    Main_Init();
}

void _epilog() {
}

void _unresolved() {
}
