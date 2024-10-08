#!/bin/bash
set -euo pipefail
IFS=$'\n\t'

script_dir=$(cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd)
cd "$script_dir"

target_dir="$script_dir/target/powerpc-unknown-eabi/release"
linker_flags=(-r -e _prolog -u _prolog -u _epilog -u _unresolved "-Wl,--gc-sections" -nostdlib -g -mno-sdata -mgcn -DGEKKO -mcpu=750 -meabi -mhard-float)

cargo +nightly build -Z build-std=core,alloc --target powerpc-unknown-eabi.json --release
"$DEVKITPPC/bin/powerpc-eabi-gcc" "${linker_flags[@]}" -o smb2-practice-mod.elf "$target_dir/libsmb2_practice_mod.a"
"$script_dir/3rdparty/ttyd-tools/ttyd-tools/elf2rel/build/elf2rel" "smb2-practice-mod.elf" -s "$script_dir/cppsrc/mkb/mkb2.us.lst" --rel-version 2 --rel-id 101
/usr/bin/env python3 "$script_dir/3rdparty/ttyd-tools/ttyd-tools/gcipack/gcipack.py" "smb2-practice-mod.rel" rel "Super Monkey Ball 2" "SMB2 Practice Mod" "$script_dir/images/banner_us.raw" "$script_dir/images/icon_us.raw" GM2E8P
