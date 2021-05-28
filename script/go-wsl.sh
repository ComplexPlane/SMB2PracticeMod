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

mkdir -p /c/Users/ComplexPlane/Documents/Dolphin\ Emulator/GC/USA/Card\ B/

if [[ "${1:-}" == "-a" ]]; then
  echo 'Building all versions'
  make -j"$(nproc)"

  echo 'Copying GCIs to dolphin emu folder'
  cp ApeSphere.*.gci /c/Users/ComplexPlane/Documents/Dolphin\ Emulator/GC/USA/Card\ B/

else
  echo 'Building us version'
  make us -j"$(nproc)"

  echo 'Copying GCI to dolphin emu folder'
  cp ApeSphere.GM2E8P.gci /c/Users/ComplexPlane/Documents/Dolphin\ Emulator/GC/USA/Card\ B/
fi

# echo 'Copying REL to smb2mut'
# cp ApeSphere.us.rel ~/romhack/smb2mut/files/mkb2.rel_sample.rel
