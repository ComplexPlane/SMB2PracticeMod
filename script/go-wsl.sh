#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'


PPCINJECT=~/romhack/util/ppc-inject/PPCInject 
DOL_SRC=~/romhack/smb2imm/sys/main.dol
DOL_DEST=~/romhack/smb2mut/sys/main.dol
RELLOADER_SRC=relloader/src/iso-rel-loader-us.asm

# if [[ "${1:-}" == "-i" ]]; then
#     echo 'Patching main.dol with ppcinject'
#     "$PPCINJECT" "$DOL_SRC" "$DOL_DEST" "$RELLOADER_SRC"
# fi

make us -j$(nproc)

# echo 'Copying REL to smb2mut'
# cp ApeSphere.us.rel ~/romhack/smb2mut/files/mkb2.rel_sample.rel

echo 'Copying GCI to dolphin emu folder' 
mkdir -p /c/Users/ComplexPlane/Documents/Dolphin\ Emulator/GC/USA/Card\ B/
cp ApeSphere.us.gci /c/Users/ComplexPlane/Documents/Dolphin\ Emulator/GC/USA/Card\ B/
