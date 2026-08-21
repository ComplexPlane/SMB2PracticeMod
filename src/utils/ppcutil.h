#pragma once

#include "utils/base.h"

/*
 * Static inline functions and enums for more readable PPC instructions.
 * Credits to CraftedCart for writing these
 */

typedef enum {
    PPC_R0 = 0,
    PPC_R1 = 1,
    PPC_R2 = 2,
    PPC_R3 = 3,
    PPC_R4 = 4,
    PPC_R5 = 5,
    PPC_R6 = 6,
    PPC_R7 = 7,
    PPC_R8 = 8,
    PPC_R9 = 9,
    PPC_R10 = 10,
    PPC_R11 = 11,
    PPC_R12 = 12,
    PPC_R13 = 13,
    PPC_R14 = 14,
    PPC_R15 = 15,
    PPC_R16 = 16,
    PPC_R17 = 17,
    PPC_R18 = 18,
    PPC_R19 = 19,
    PPC_R20 = 20,
    PPC_R21 = 21,
    PPC_R22 = 22,
    PPC_R23 = 23,
    PPC_R24 = 24,
    PPC_R25 = 25,
    PPC_R26 = 26,
    PPC_R27 = 27,
    PPC_R28 = 28,
    PPC_R29 = 29,
    PPC_R30 = 30,
    PPC_R31 = 31,
} PPC_Register;

static inline u32 PPC_InstrB(u32 target) {
    return 0x48000000 + (target & 0x3FFFFFF);
}

static inline u32 PPC_InstrBA(u32 target) {
    return 0x48000002 + (target & 0x3FFFFFF);
}

static inline u32 PPC_InstrBL(u32 target) {
    return 0x48000001 + (target & 0x3FFFFFF);
}

static inline u32 PPC_InstrBLA(u32 target) {
    return 0x48000003 + (target & 0x3FFFFFF);
}

static inline u32 PPC_InstrBLR(void) {
    return 0x4E800020;
}

static inline u32 PPC_InstrLI(PPC_Register dest_register, u16 value) {
    return 0x38000000 + (((u32)dest_register) << 21) + value;
}

static inline u32 PPC_InstrLIS(PPC_Register dest_register, u16 value) {
    return 0x3C000000 + (((u32)dest_register) << 21) + value;
}

static inline u32 PPC_InstrNOP(void) {
    return 0x60000000;
}

// TODO: PPC_INSR_CMPWI
