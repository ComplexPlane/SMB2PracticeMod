#pragma once

/*
 * Macros for more readable PPC instructions.
 * Credits to CraftedCart for writing these
 */

#define PPC_INSTR_B(target) (0x48000000 + (((u32)(target)) & 0x3FFFFFF))
#define PPC_INSTR_BA(target) (0x48000002 + (((u32)(target)) & 0x3FFFFFF))
#define PPC_INSTR_BL(target) (0x48000001 + (((u32)(target)) & 0x3FFFFFF))
#define PPC_INSTR_BLA(target) (0x48000003 + (((u32)(target)) & 0x3FFFFFF))
#define PPC_INSTR_BLR() (0x4E800020)

#define PPC_INSTR_LI(dest_register, value) \
    (0x38000000 + (((u32)(dest_register)) << 21) + ((u16)value))
#define PPC_INSTR_LIS(dest_register, value) \
    (0x3C000000 + (((u32)(dest_register)) << 21) + ((u16)value))

#define PPC_INSTR_NOP() (0x60000000)

// TODO: PPC_INSR_CMPWI

#define PPC_R0 0
#define PPC_R1 1
#define PPC_R2 2
#define PPC_R3 3
#define PPC_R4 4
#define PPC_R5 5
#define PPC_R6 6
#define PPC_R7 7
#define PPC_R8 8
#define PPC_R9 9
#define PPC_R10 10
#define PPC_R11 11
#define PPC_R12 12
#define PPC_R13 13
#define PPC_R14 14
#define PPC_R15 15
#define PPC_R16 16
#define PPC_R17 17
#define PPC_R18 18
#define PPC_R19 19
#define PPC_R20 20
#define PPC_R21 21
#define PPC_R22 22
#define PPC_R23 23
#define PPC_R24 24
#define PPC_R25 25
#define PPC_R26 26
#define PPC_R27 27
#define PPC_R28 28
#define PPC_R29 29
#define PPC_R30 30
#define PPC_R31 31
