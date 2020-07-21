.global StageSelectMenuHook
.global PauseMenuTextHook

StageSelectMenuHook: // Hook at 0x80274804
stwu r1, -0x38 (r1)
stw r31, 0x34 (r1)
stw r30, 0x30 (r1)
stw r29, 0x2c (r1)
stw r28, 0x28 (r1)
stw r27, 0x24 (r1)
stw r26, 0x20 (r1)
stw r25, 0x1c (r1)
stw r24, 0x18 (r1)
stw r23, 0x14 (r1)
stw r22, 0x10 (r1)
stw r21, 0xc (r1)
mr r31, r12
mr r30, r11
mr r29, r10
mr r28, r9
mr r27, r8
mr r26, r7
mr r25, r6
mr r24, r5
mr r23, r4
mr r22, r3
mr r21, r0

bl IsIWComplete
cmpwi r3, 1
bne IwNotComplete
// Jump to "Exit game" handler
lis r4, 0x8027
ori r4, r4, 0x48cc
mtlr r4
b EndStageSelectMenuHook

// Resume "Stage select" handler
IwNotComplete:
lis r4, 0x8027
ori r4, r4, 0x4808
mtlr r4
li r21, 6 // Replaced instruction was "li r0, 6"

EndStageSelectMenuHook:
mr r12, r31
mr r11, r30
mr r10, r29
mr r9, r28
mr r8, r27
mr r7, r26
mr r6, r25
mr r5, r24
mr r4, r23
mr r3, r22
mr r0, r21
lwz r31, 0x34 (r1)
lwz r30, 0x30 (r1)
lwz r29, 0x2c (r1)
lwz r28, 0x28 (r1)
lwz r27, 0x24 (r1)
lwz r26, 0x20 (r1)
lwz r25, 0x1c (r1)
lwz r24, 0x18 (r1)
lwz r23, 0x14 (r1)
lwz r22, 0x10 (r1)
lwz r21, 0xc (r1)
addi r1, r1, 0x38
blr

// ------------------------------------------------------------------------------------------

PauseMenuTextHook: // Hook at 0x8032a86c
stwu r1, -0x38 (r1)
stw r31, 0x34 (r1)
stw r30, 0x30 (r1)
stw r29, 0x2c (r1)
stw r28, 0x28 (r1)
stw r27, 0x24 (r1)
stw r26, 0x20 (r1)
stw r25, 0x1c (r1)
stw r24, 0x18 (r1)
stw r23, 0x14 (r1)
stw r22, 0x10 (r1)
stw r21, 0xc (r1)
mr r31, r12
mr r30, r11
mr r29, r10
mr r28, r9
mr r27, r8
mr r26, r7
mr r25, r6
mr r24, r5
mr r23, r4
mr r22, r3
mr r21, r0

bl IsIWComplete
cmpwi r3, 1
bne DontModifyText

// Check if we're trying to draw the "Stage select" string
lwzx r3, r25, r21 // Load pointer to string like in replaced instruction
lis r4, 0x8047
ori r4, r4, 0xf02c
cmpw r3, r4
bne DontModifyText

// Replace pointer with pointer to our custom text
lis r23, FINISH_IW_MSG@h
ori r23, r23, FINISH_IW_MSG@l
b EndPauseMenuTextHook

// Show current menu text like normal
DontModifyText:
lwzx r23, r25, r21 // Analog of replaced instruction

EndPauseMenuTextHook:
// Resume to 0x8032a870
lis r3, 0x8032
ori r3, r3, 0xa870
mtctr r3
mr r12, r31
mr r11, r30
mr r10, r29
mr r9, r28
mr r8, r27
mr r7, r26
mr r6, r25
mr r5, r24
mr r4, r23
mr r3, r22
mr r0, r21
lwz r31, 0x34 (r1)
lwz r30, 0x30 (r1)
lwz r29, 0x2c (r1)
lwz r28, 0x28 (r1)
lwz r27, 0x24 (r1)
lwz r26, 0x20 (r1)
lwz r25, 0x1c (r1)
lwz r24, 0x18 (r1)
lwz r23, 0x14 (r1)
lwz r22, 0x10 (r1)
lwz r21, 0xc (r1)
addi r1, r1, 0x38
bctr

// ------------------------------------------------------------------------------------------

IsIWComplete:

// Check that we're not in Practice Mode
lis r3, 0x805d
ori r3, r3, 0x4914
lwz r4, 0x0 (r3)
cmpwi r4, 1
beq No

// Check if we're doing an IW
lis r3, currentlyPlayingIW@h
ori r3, r3, currentlyPlayingIW@l
lwz r4, 0x0 (r3)
cmpwi r4, 1
bne No

// Check if 9 stages in world complete

// Get current world number
lis r3, 0x8054
ori r3, r3, 0xdbb8
lhz r4, 0x4 (r3)

// Get current number of stages beaten in world from world number
lis r5, 0x805d
ori r5, r5, 0x4b08
mulli r4, r4, 56
add r5, r5, r4
lhz r6, 0x2 (r5)
cmpwi r6, 9
bne No

// Check if current stage complete
lis r3, 0x8054
ori r3, r3, 0xdc83
lbz r4, 0x0 (r3)
cmpwi r4, 1
bne No

li r3, 1
b EndIsIWComplete

No:
li r3, 0

EndIsIWComplete:
blr

// ------------------------------------------------------------------------------------------
FINISH_IW_MSG: .ascii "Finish IW"
