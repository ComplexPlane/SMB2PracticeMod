#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

PPCINJECT=~/romhack/util/ppc-inject/PPCInject 
DOL_SRC=~/romhack/smb2imm/sys/main.dol
DOL_DEST=~/romhack/smb2mut/sys/main.dol
RELLOADER_SRC=relloader/src/iso-rel-loader-us.asm
DOLPHIN_DIR=~/.var/app/org.DolphinEmu.dolphin-emu/data/dolphin-emu 

echo "Building"
make us -j

echo 'Copying GCI to dolphin emu folder' 
mkdir -p "$DOLPHIN_DIR/GC/USA/Card B"
cp SMB2PracticeMod.GM2E8P.gci "$DOLPHIN_DIR/GC/USA/Card B"
