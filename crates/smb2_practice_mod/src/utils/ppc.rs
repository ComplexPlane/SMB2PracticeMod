/*
 * Utils for more readable PPC instructions.
 * Credits to CraftedCart for writing these
 */

pub fn instr_li(dest_register: Reg, value: u16) -> usize {
    0x38000000 + ((dest_register as usize & 0x1F) << 21) + (value as usize)
}

pub const B_OPCODE_MASK: usize = 0xFC000000;
pub const B_OPCODE: usize = 0x48000000;
pub const B_DEST_MASK: usize = 0x03FFFFFC;

#[allow(dead_code)]
pub enum Reg {
    R0 = 0,
    R1 = 1,
    R2 = 2,
    R3 = 3,
    R4 = 4,
    R5 = 5,
    R6 = 6,
    R7 = 7,
    R8 = 8,
    R9 = 9,
    R10 = 10,
    R11 = 11,
    R12 = 12,
    R13 = 13,
    R14 = 14,
    R15 = 15,
    R16 = 16,
    R17 = 17,
    R18 = 18,
    R19 = 19,
    R20 = 20,
    R21 = 21,
    R22 = 22,
    R23 = 23,
    R24 = 24,
    R25 = 25,
    R26 = 26,
    R27 = 27,
    R28 = 28,
    R29 = 29,
    R30 = 30,
    R31 = 31,
}
