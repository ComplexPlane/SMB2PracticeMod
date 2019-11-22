# ApeSphere REL Loader
# Based on the TTYD-Practice-Codes REL loader written by PistonMiner and Zephiles

# 80006d10 - Address to hook for this code
# This is after mkb2.main_loop.rel is loaded and linked, but before its prolog runs

# Stack Layout
# No heap is available, so we use the stack for temporary space
# | Size                   | Offset | Size   | Alignment |
# | ---------------------- | ------ | ------ | --------- |
# | CARDMount work area    | 0x220  | 0xa000 | 32        |
# | Small card file buffer | 0x20   | 0x200  | 32        |
# | CardFileInfo           | 0xc    | 0x14   |           |
# | "rel" string           | 0x8    | 0x4    |           |
# | LR Save                | 0x4    | 0x4    |           |
# | SP Save                | 0x0    | 0x4    |           |

# Register Assignments
# r31: Used to preserve r12 from hook point, which is the prolog function of the main_loop rel
# r30: REL prolog pointer, initially 0. Keeps track of whether a REL is loaded or not
# r29: CardFileInfo struct buffer, allocated on stack
# r28: REL module size, rounded up to multiple of 0x200 bytes
# r27: Card file buffer, allocated on stack
# r26: CARDMount work area, allocated on stack
# r25: REL module region, allocated on arena
# r24: REL BSS buffer, allocated on arena
# r23: Original arena low pointer
# r22: Pointer to "rel" filename on stack
# r21: Place for finishAsyncFunction to save LR temporarily

# Save r12 from hook point
mr r31,r12

# Used to check if a REL file is loaded or not
li r30,0

# Make stack frame
lis r3,0xffff
ori r3,r3,0x5de0 # -0xa220
stwux r1,r1,r3

# Initialize stack buffer pointers
addi r26,r1,0x220 # CARDMount work area
addi r27,r1,0x20 # Small card file buffer
addi r29,r1,0xc # CardFileInfo
addi r22,r1,0x8 # "rel"

# Write "rel" string (file name) to stack
lis r3,0x7265
ori r3,r3,0x6c00
stw r3,0x0(r22)

# Save original arena low pointer
lis r3,0x8000
ori r3,r3,0xd590 # OSGetArenaLo
mtctr r3
bctrl
mr r23,r3 # Save address for later

# Check if a memory card is present
# To avoid an infinite loop, only check for a memory card a set amount of times
lis r6,0x000F
ori r6,r6,0x4240 # Check 1,000,000 times

checkForMemoryCardLoop:
lis r3,0x8002
ori r3,r3,0x611c # CARDProbeEx
mtctr r3
li r3,1 # Memory card slot B
li r4,0 # Pointer to the memory size (not needed, so NULL)
li r5,0 # Pointer to the sector size (not needed, so NULL)
bctrl
cmpwi r3,-1 # CARD_RESULT_BUSY
bne- exitCheckForMemoryCardLoop

# Exit if the limit has been reached
subic. r6,r6,1
bgt+ checkForMemoryCardLoop
b exit

exitCheckForMemoryCardLoop:
cmpwi r3,0 # Check if something went wrong
bne- exit

# Mount the memory card
lis r3,0x8002
ori r3,r3,0x67b0 # CARDMountAsync
mtctr r3
li r3,1 # Memory card slot B
mr r4,r26 # Pointer to the card work area
li r5,0 # Detach callback function (want to avoid using, so NULL)
li r6,0 # Attach Callback function (want to avoid using, so NULL)
bctrl
bl finishAsyncFunction
cmpwi r3,0 # Check if something went wrong
bne- exit

# Open the file
lis r3,0x8002
ori r3,r3,0x74fc # CARDOpen
mtctr r3
li r3,1 # Memory card slot B
mr r4,r22 # File name ("rel" on stack)
mr r5,r29 # Pointer to CardFileInfo
bctrl
cmpwi r3,0 # Check if something went wrong
bne- unmountMemoryCard

# Read from the card
lis r3,0x8002
ori r3,r3,0x7cb4 # CARDReadAsync
mtctr r3
mr r3,r29 # Pointer to CardFileInfo
mr r4,r27 # Pointer to the file buffer
li r5,0x200 # Amount of bytes to read
li r6,0x2000 # Offset in the file to start reading from
li r7,0 # Read callback function (want to avoid using, so NULL)
bctrl
bl finishAsyncFunction
cmpwi r3,0 # Check if something went wrong
bne- unmountMemoryCard

# Get the REL module size and adjust it to be in multiples of 0x200 bytes
lwz r28,0x40(r27) # Module size
addi r28,r28,511
rlwinm r28,r28,0,0,22

# Allocate buffer for the REL based on the adjusted module size
mr r3,r28
bl allocateOnArena
mr r25,r3 # Backup for later

# Read the REL area of the file
lis r3,0x8002
ori r3,r3,0x7cb4 # CARDReadAsync
mtctr r3
mr r3,r29 # Pointer to CardFileInfo
mr r4,r25 # Pointer to the REL module buffer
mr r5,r28 # Adjusted REL module size
li r6,0x2200 # Offset in the file to start reading from
li r7,0 # Read callback function (want to avoid using, so NULL)
bctrl
bl finishAsyncFunction
cmpwi r3,0 # Check if something went wrong
bne- restoreArena

# Get the BSS area size and allocate memory for it
lwz r3,0x20(r25)
bl allocateOnArena
mr r24,r3 # Backup for later

# Overwrite the REL version from 3 to 2.
# elf2rel produces RELs with version 3, but SMB2 only accepts rels with version < 3
# TODO support v2 in elf2rel?
li r3,0x2
stw r3,0x1c(r25)

# Link the functions in the REL
lis r3,0x8001
ori r3,r3,0x0730 # OSLink
mtctr r3
mr r3,r25 # Pointer to the REL module buffer
mr r4,r24 # Pointer to the BSS area
bctrl
cmpwi r3,1 # Check if something went wrong
bne- unlinkRel

lwz r3,0x34(r25) # Get the REL prolog pointer
cmpwi r3,0
beq- unlinkRel # If it's 0 for some reason, unlinkRel
mr r30,r3 # Store REL prolog pointer

# Done loading REL, so unmount memory card, run prolog, and exit
b unmountMemoryCard

# 
# Common functions
#

allocateOnArena:
lis r4,0x8000
ori r4,r4,0xd5a8 # OSAllocFromArenaLow
mtctr r4
addi r3,r3,31
rlwinm r3,r3,0,0,26 # Amount to allocate, rounded up to multiple of 32 bytes
li r4,32 # Alignment
bctr

finishAsyncFunction:
cmpwi r3,0 # Check if something went wrong
bnelr-
mflr r21

finishAsyncFunctionLoop:
lis r3,0x8002
ori r3,r3,0x33e4 # CARDGetResultCode
mtctr r3
li r3,1 # Memory card slot B
bctrl
cmpwi r3,-1 # CARD_RESULT_BUSY
beq+ finishAsyncFunctionLoop
mtlr r21
blr

#
# Error handling branch points
#

# Unlink the REL if it failed to link
unlinkRel:
lis r3,0x8001
ori r3,r3,0x0b8c # OSUnlink
mtctr r3
mr r3,r25 # Pointer to the module
bctrl

# Restore the arena low pointer if we cannot load the REL
restoreArena:
lis r3,0x8000
ori r3,r3,0xd5a0 # OSSetArenaLo
mtctr r3
mr r3,r23
bctrl

#
# Unmount memory card, run REL prolog, and exit
#

# Unmount the Memory card
#
# We don't call CARDClose() on our file because this function seemingly does
# not appear in SMB2. Since ths function is never called by the game, it must
# not be _that_ big of a deal to unmount the card without closing the file.
unmountMemoryCard:
lis r3,0x8002
ori r3,r3,0x69ec # CARDUnmount
mtctr r3
li r3,1 # Memory card slot B
bctrl

# Run the REL prolog if the load was successful
cmpwi r30,0
beq- exit
mtctr r30
bctrl

# Restore stack pointer
exit:
lwz r1,0x0(r1)

# Restore intruction replaced by hook
# Original instruction was: mtctr r12
mtctr r31
