.global stage_select_menu_hook
.global pause_menu_text_hook
.global is_iw_complete

.text

stage_select_menu_hook: // Hook at 0x80274804
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

bl is_iw_complete
cmpwi r3, 1
bne iw_not_complete
// Jump to "Exit game" handler
lis r4, stage_select_menu_exit_handler@ha
ori r4, r4, stage_select_menu_exit_handler@l
mtlr r4
b end_stage_select_menu_hook

// Resume "Stage select" handler
iw_not_complete:
lis r4, stage_select_menu_resume@ha
ori r4, r4, stage_select_menu_resume@l
mtlr r4
li r21, 6 // Replaced instruction was "li r0, 6"

end_stage_select_menu_hook:
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

pause_menu_text_hook: // Hook at 0x8032a86c
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

bl is_iw_complete
cmpwi r3, 1
bne dont_modify_text

// Check if we're trying to draw the "Stage select" string
lwzx r3, r25, r21 // Load pointer to string like in replaced instruction
lis r4, PAUSEMENU_STAGE_SELECT_STRING@ha
ori r4, r4, PAUSEMENU_STAGE_SELECT_STRING@l
cmpw r3, r4
bne dont_modify_text

// Replace pointer with pointer to our custom text
lis r23, FINISH_IW_MSG@ha
ori r23, r23, FINISH_IW_MSG@l
b end_pause_menu_text_hook

// Show current menu text like normal
dont_modify_text:
lwzx r23, r25, r21 // Analog of replaced instruction

end_pause_menu_text_hook:
// Resume to 0x8032a870
lis r3, pause_menu_text_hook_resume@ha
ori r3, r3, pause_menu_text_hook_resume@l
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

is_iw_complete:

// Check that we're not in Practice Mode
lis r3, in_practice_mode@ha
lwz r4, in_practice_mode@l(r3)
cmpwi r4, 1
beq No

// Check if we're doing an IW
lis r3, currently_playing_iw@ha
lwz r4, currently_playing_iw@l(r3)
cmpwi r4, 0
beq No

// Check if 9 stages in world complete

// Get current world number
lis r3, scen_info+4@ha
lhz r4, scen_info+4@l(r3)

// Get current number of stages beaten in world from world number
lis r5, sm_state@ha
ori r5, r5, sm_state@l
mulli r4, r4, 56
add r5, r5, r4
lhz r6, 0x2 (r5)
cmpwi r6, 9
bne No

// Check if current stage complete
lis r3, stage_complete@ha
lbz r4, stage_complete@l(r3)
cmpwi r4, 1
bne No

// Check if we're in the Scenario submode (aka, level select menu is open)
// Need to check this because the level completion flag carries over to this menu from the previous stage
lis r3, sub_mode@ha
lwz r4, sub_mode@l(r3)
cmpwi r4, 87
beq No
cmpwi r4, 88
beq No

li r3, 1
b end_is_iw_complete

No:
li r3, 0

end_is_iw_complete:
blr

.data

// ------------------------------------------------------------------------------------------
FINISH_IW_MSG: .ascii "Finish IW"
