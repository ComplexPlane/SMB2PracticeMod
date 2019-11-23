# 80006d08 - Address to hook for this code

# Make sure mkb2.main_game.rel was the REL file that was just loaded
lwz r3,0(r31) # Module
lwz r3,0(r3) # Module Id
cmpwi r3,0x1
bne+ exit

# Initialize static values used for addresses
# Do not overwrite r31
lis r30,0x8002
lis r29,0x8001
lis r28,0x8000

li r27,0 # Used to check if a REL file is loaded or not

# Check if a memory card is present
# To avoid an infinite loop, only check for a memory card a set amount of times
lis r26,0x000f
ori r26,r26,0x4240 # Check 1,000,000 times

checkForMemoryCardLoop:
ori r3,r30,0x611c # CARDProbeEx
mtctr r3
li r3,0 # Memory card slot A
li r4,0 # Pointer to the memory size (not needed, so NULL)
li r5,0 # Pointer to the sector size (not needed, so NULL)
bctrl
cmpwi r3,-1 # CARD_RESULT_BUSY
bne- exitCheckForMemoryCardLoop

# Exit if the limit has been reached
subic. r26,r26,1
bgt+ checkForMemoryCardLoop

exitCheckForMemoryCardLoop:
cmpwi r3,0 # Check if something went wrong
bne- exit

# Back up the current Arena Low
lwz r25,-0x7f50(r13) # Arena Low

# Allocate memory for the CARDMount work area, CardFileInfo, 
# and the initial 0x200 bytes for reading from the memory card
ori r3,r27,0xa220
bl allocateFromArenaLow

# Backup the returned address to be used for later
mr r26,r3

# Mount the Memory Card
ori r3,r30,0x67b0 # CARDMountAsync
mtctr r3
li r3,0 # Memory Card Slot A
addi r4,r26,0x220 # Pointer to the Work Area
li r5,0 # Detach Callback function (Want to avoid using, so NULL)
li r6,0 # Attach Callback function (Want to avoid using, so NULL)
bctrl
bl finishAsyncFunction
cmpwi r3,0 # Check if something went wrong
bne- freeMemory

# Open the file
ori r3,r30,0x74fc # CARDOpen
mtctr r3
li r3,0 # Memory Card Slot A
ori r4,r28,0x44c0 # File Name
addi r5,r26,0 # Pointer to CardFileInfo
bctrl
cmpwi r3,0 # Check if something went wrong
bne- unmountCard

# Read from the Card
ori r3,r30,0x7cb4 # CARDReadAsync
mtctr r3
addi r3,r26,0 # Pointer to CardFileInfo
addi r4,r26,0x20 # Pointer to the File Buffer
li r5,0x200 # Amount of bytes to read
li r6,0x2000 # Offset in the file to start reading from
li r7,0 # Read Callback function (Want to avoid using, so NULL)
bctrl
bl finishAsyncFunction
cmpwi r3,0 # Check if something went wrong
bne- closeCard

# Get the file size and adjust it to be in multiples of 0x200 bytes
lwz r24,0x60(r26) # File Size
addi r24,r24,511
rlwinm r24,r24,0,0,22

# Get the main loop's Relocation Data location
lwz r23,0(r31) # Module
lwz r23,0x24(r23) # Relocation Data

# Round the Relocation Data address up to the nearest multiple of 0x20 bytes
addi r23,r23,31
rlwinm r23,r23,0,0,26

# Allocate memory based on the adjusted file size
mr r3,r24 # File Size
bl allocateFromMainLoopRelocMemory

# Backup the returned address to be used for later
mr r22,r3

# Read the REL Area of the file
ori r3,r30,0x7cb4 # CARDReadAsync
mtctr r3
addi r3,r26,0 # Pointer to CardFileInfo
mr r4,r22 # Pointer to the File Buffer
mr r5,r24 # Adjusted File Size
li r6,0x2200 # Offset in the file to start reading from
li r7,0 # Read Callback Function (Want to avoid using, so NULL)
bctrl
bl finishAsyncFunction
cmpwi r3,0 # Check if something went wrong
bne- closeCard

# Get the BSS Area size and allocate memory for it
lwz r3,0x20(r22)
bl allocateFromMainLoopRelocMemory

# Backup the returned address to be used for later
mr r21,r3

# Link the functions in the REL
ori r3,r29,0x730 # OSLink
mtctr r3
mr r3,r22 # Pointer to the Module
mr r4,r21 # Pointer to the BSS Area
bctrl
cmpwi r3,1 # Check if something went wrong
bne- callOSUnlink

# Store the Main Loop Module, the Relocation Data size, the next
# free spot in Relocation Data, the BSS Area, and the REL Module
lwz r3,0(r31) # Module
stw r3,0x4528(r28)
stw r23,0x452c(r28) # Relocation Data
stw r21,0x4530(r28) # Pointer to the BSS Area
stw r22,0x4534(r28) # Pointer to the Module

# Get the REL Prolog Pointer
lwz r27,0x34(r22)

# Done, so close and unmount the card
b closeCard

allocateFromMainLoopRelocMemory:
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)
stmw %r30,0x8(%sp)

addi r4,r3,31
rlwinm r4,r4,0,0,26 # Round the size up to the nearest multiple of 0x20 bytes
mr r31,r4 # Size
mr r3,r23 # Pointer to Relocation Data

# Set the next free spot in the Relocation Data
add r23,r23,r4

clearAndFlushMemory:
ori r5,r28,0x33a8 # memset
mtctr r5
mr r30,r3 # Dest
li r4,0
mr r5,r31 # Size
bctrl

# Flush the memory
ori r3,r28,0xd8cc # DCFlushRange
mtctr r3
mr r3,r30 # Dest
mr r4,r31 # Size
bctrl
mr r3,r30 # Dest

lmw %r30,0x8(%sp)
lwz %r0,0x14(%sp)
mtlr %r0
addi %sp,%sp,0x10
blr

allocateFromArenaLow:
stwu %sp,-0x10(%sp)
mflr %r0
stw %r0,0x14(%sp)
stmw %r30,0x8(%sp)

ori r4,r28,0xd5a8 # OSAllocFromArenaLow
mtctr r4
addi r3,r3,31
rlwinm r3,r3,0,0,26 # Round the size up to the nearest multiple of 0x20 bytes
mr r31,r3 # Size
li r4,32 # Alignment
bctrl

# Clear and flush the allocated memory
b clearAndFlushMemory

finishAsyncFunction:
cmpwi r3,0 # Check if something went wrong
bnelr-
mflr r14

finishAsyncFunctionLoop:
ori r3,r30,0x33e4 # CARDGetResultCode
mtctr r3
li r3,0 # Memory Card Slot A
bctrl
cmpwi r3,-1 # CARD_RESULT_BUSY
beq+ finishAsyncFunctionLoop
mtlr r14
blr

CARDClose:
mflr r0
stw r0,0x4(sp)
stwu sp,-0x18(sp)
stw r31,0x14(sp)
mr r31,r3

ori r3,r30,0x32c8 # __CARDGetControlBlock
mtctr r3
addi r4,sp,0xc
lwz r3,0(r31)
bctrl

cmpwi r3,0
bge- noProblemsFound
b exitCloseCard

noProblemsFound:
ori r3,r30,0x3380 # __CARDPutControlBlock
mtctr r3
li r0,-1
stw r0,0(r31)
li r4,0
lwz r3,0xc(sp)
bctrl

exitCloseCard:
lwz r0,0x1c(sp)
lwz r31,0x14(sp)
addi sp,sp,0x18
mtlr r0
blr

callOSUnlink:
ori r3,r29,0xb8c # OSUnlink
mtctr r3
mr r3,r22 # Pointer to the Module
bctrl

closeCard:
addi r3,r26,0 # Pointer to CardFileInfo
bl CARDClose

unmountCard:
ori r3,r30,0x69ec # CARDUnmount
mtctr r3
li r3,0 # Memory Card Slot A
bctrl

freeMemory:
# Restore the Arena Low
stw r25,-0x7f50(r13) # Arena Low

# Run the REL Prolog if the load was successful
cmpwi r27,0
beq- exit
mtctr r27
bctrl

exit:
# Restore the overwritten instruction
lwz r3,0(r31) # Module
