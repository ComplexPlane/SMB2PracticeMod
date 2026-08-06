#include "main.h"

#include "mkb/mkb2_ghidra.h"

void _prolog() {
    main_init();
}

void _epilog() {
}

void _unresolved() {
    mkb_OSPanic("", 0, "[pracmod] Called an unlinked function, aborting now");
    while (true)
        ;
}
