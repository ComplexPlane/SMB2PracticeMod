#include "systems/main.h"

#include "mkb/mkb2_ghidra.h"

void _prolog() {
    // Run mod init function
    main_init();
}

void _epilog() {
}

void _unresolved() {
}
