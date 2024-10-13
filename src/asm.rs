use core::arch::global_asm;

extern "C" {
    pub static custom_titlescreen_text_color: u8;
    pub static full_debug_text_color: u8;
    pub static mut debug_text_color: u32;
}

global_asm!(
    ".global custom_titlescreen_text_color

    # Hooked at 0x8032ad0c GC
    custom_titlescreen_text_color:

    # Load original address of 'PRESS START/PAUSE' string
    lis %r5, 0x8047
    ori %r5, %r5, 0xf4ec

    # Calculate the index of the current character
    sub %r6, %r30, %r5
    cmpwi %r6, 5
    bge color2

    color1:
    lis %r6, 0xfff0
    ori %r6, %r6, 0xf000
    b end1

    color2:
    lis %r6, 0xb15a
    ori %r6, %r6, 0xff00

    end1:
    stw %r6, 0x8(%r1)
    lis %r5, 0x8032
    ori %r5, %r5, 0xad3c
    mtctr %r5
    bctr",
);

global_asm!(
    ".global full_debug_text_color

    # Hooked at 0x802aeca4 GC
    full_debug_text_color:

    lis %r5, debug_text_color@h
    ori %r5, %r5, debug_text_color@l
    lwz %r6, 0(%r5)
    cmpwi %r6, 0
    beq end2
    mr %r4, %r5

    end2:
    li %r3, 1 # Overwritten instruction
    lis %r5, 0x802a
    ori %r5, %r5, 0xeca8
    mtctr %r5
    bctr

    .section .data
    .align 4
    .global debug_text_color
    debug_text_color:
    .long 0
"
);
