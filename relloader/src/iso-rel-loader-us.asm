#function $filename 0xa6a5c
0x6D6B6232              % mkb2
0x2E72656C              % .rel
0x5F73616D              % _sam
0x706C652E              % ple.
0x72656C00              % rel\0

#function $injection 0x2ae8
0x480a2a58              % bl $relLoader

#function $relLoader 0xa6760 
% Make sure mkb2.main_game.rel was the REL file that was just loaded
lwz r3,0(r31)           % Pointer to the Main Loop's Module
lwz r3,0(r3)            % Pointer to the Main Loop's Module Id
cmpwi r3,0x1
bne .exit

% Initialize static values used for addresses
% Do not overwrite r31
lis r30,0x8002
lis r29,0x8001
lis r28,0x8000

li r27,0                % Used to check if a REL file is loaded or not

% Back up the current Arena Low
lwz r25,-0x7f50(r13)    % Arena Low

% Allocate memory for the CARDMount work area, CardFileInfo, 
% and the initial 0x200 bytes for reading from the memory card
ori r3,r27,0xa220
bl .allocateFromArenaLow

% Backup the returned address to be used for later
mr r26, r3

% Open file on DVD
ori r3, r29, 0x87a0     % DVDOpen
0x7c6903a6              % mtctr r3
lis r3, 0x800a
ori r3, r3, 0x9a5c      % File name pointer (argument 1)
mr r4, r26              % File info pointer (argument 2)
0x4e800421              % bctrl
cmpwi r3, 0
beq .exit

% Get filesize
lwz r24, 0x34(r26)
addi r24, r24, 31       % add 31 to round down for 32-byte alignment
rlwinm r24, r24,0,0,26  % 32-byte alignment w/ mask 0xffffffe0
mr r3, r24      

% Allocate memory for the file
lwz r23, 0(r31)         % ptr to main loop module
lwz r23, 0x24(r23)      % ptr to relocation data
addi r23, r23, 31       
rlwinm r23, r23, 0, 0, 26 % round to nearest 0x20
mr r3, r24              % file size
bl .allocateFromMainLoopRelocMemory
mr r22, r3              % move returned address to r22

% Read file from DVD
ori r3, r28, 0x92fc     % read_entire_file_using_dvdread_prio_async
0x7c6903a6              % mtctr r3
mr r3, r26              % File info pointer (argument 1)
mr r4, r22              % File buffer (argument 2)
mr r5, r24              % File size (argument 3)
li r6, 0x0              % File offset (argument 4)
0x4e800421              % bctrl r3

% Allocate memory for BSS area
lwz r3, 0x20(r22)       % get BSS area size
bl .allocateFromMainLoopRelocMemory
mr r21, r3              % save BSS ptr in r21

% OSLink
ori r3, r29, 0x730      % OSLInk
0x7c6903a6              % mtctr r3
mr r3, r22              % module ptr
mr r4, r21              % BSS area
0x4e800421              % bctrl
cmpwi r3, 0x1           % check for errors
bne .callOSUnlink

lwz r3, 0(r31)          % ptr to main loop's module
stw r3, 0x4524(r28)
lwz r3, 0x4(r31)        % ptr to main loop's BSS area
stw r3, 0x4528(r28)   
stw r23, 0x452c(r28)    % ptr to main loop's relocation data
stw r21, 0x4530(r28)    % ptr to BSS area
stw r22, 0x4532(r28)    % ptr to module

lwz r27, 0x34(r22)      % prolog ptr

b .DVDClose

.callOSUnlink
ori r3,r29,0xb8c        % OSUnlink
0x7c6903a6              % mtctr r3
mr r3, r22              % ptr to module
0x4e800421              % bctrl

.DVDClose
ori r3, r29, 0x8868     % DVDClose
0x7c6903a6              % mtctr r3
mr r3, r26              % file info ptr
0x4e800421              % bctrl

% Run REL prolog
.freeMemory
stw r25, -0x7f50(r13)   % restore arena low
cmpwi r27, 0
beq .exit
0x7f6903a6              % mtctr r27
0x4e800421              % bctrl
b .exit

.allocateFromArenaLow
stwu r1,-0x10(r1)
mflr r0
stw r0,0x14(r1)
0xbfc10008              % stmw r30,0x8(r1)
ori r4,r28,0xd5a8       % OSAllocFromArenaLow
0x7c8903a6              % mtctr r4
addi r3,r3,31
rlwinm r3,r3,0,0,26     % Round the size up to the nearest multiple of 0x20 bytes
mr r31,r3 % Size
li r4,32 % Alignment
0x4e800421              % bctrl
b .clearAndFlushMemory

.allocateFromMainLoopRelocMemory
stwu r1,-0x10(r1)
mflr r0
stw r0, 0x14(r1)
0xbfc10008              % stmw r30, 0x8(r1)

addi r4, r3, 31
rlwinm r4, r4, 0, 0, 26 % round to nearest multiple of 0x20
mr r31, r4              % size/amount to allocate
mr r3, r23  

add r23,r23, r4         % next spot in main loop's relocation data

.clearAndFlushMemory
ori r5,r28,0x33a8       % memset
0x7ca903a6				% mtctr r5
mr r30,r3				% Dest
li r4,0
mr r5,r31				% Size
0x4e800421				% bctrl

% Flush the memory
ori r3,r28,0xd8cc       % DCFlushRange
0x7c6903a6				% mtctr r3
mr r3,r30 % Dest
mr r4,r31 % Size
0x4e800421				% bctrl
mr r3,r30 % Dest

0xbbc10008				% lmw r30,0x8(r1)
lwz r0,0x14(r1)
mtlr r0
addi r1,r1,0x10
blr

.exit
lwz r3,0(r31)           % Pointer to the Main Loop's Moduleo (replaces overwritten instruction)
ori r9, r28, 0x6d0c     % this is done because the call stack doesn't behave like a stack for some reason? 
0x7d2803a6              % mtspr LR, r9
blr
