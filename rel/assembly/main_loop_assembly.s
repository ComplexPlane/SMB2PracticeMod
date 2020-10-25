.global start_main_loop_assembly

start_main_loop_assembly:
stwu sp, -0x10(sp)
mflr r0
stw r0, 0x14(sp)
stw r31, 0xC(sp)
mr r31, r3

bl tick

mr r3, r31
lwz r31, 0xC(sp)
lwz r0, 0x14(sp)
mtlr r0
addi sp, sp, 0x10

# Restore the overwritten instruction
lwz r0, 0(r3)
blr