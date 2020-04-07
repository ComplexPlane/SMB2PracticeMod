#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

# make us -j$(nproc) && echo 'Copying to dolphin emu folder' && cp ApeSphere.us.gci ~/.local/share/dolphin-emu/GC/USA/Card\ A/

PPCINJECT=~/romhack/util/ppc-inject/PPCInject 
DOL_SRC=~/romhack/smb2imm/sys/main.dol
DOL_DEST=~/romhack/smb2mut/sys/main.dol
RELLOADER_SRC=relloader/src/iso-rel-loader-us.asm

if [[ "${1:-}" == "-i" ]]; then
    "$PPCINJECT" "$DOL_SRC" "$DOL_DEST" "$RELLOADER_SRC"
fi

make us -j$(nproc)
cp ApeSphere.us.rel ~/romhack/smb2mut/files/
