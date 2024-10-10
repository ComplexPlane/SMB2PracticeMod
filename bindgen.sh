#!/bin/bash
set -euo pipefail
IFS=$'\n\t'

script_dir=$(cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd)
cd "$script_dir"

bindgen cppsrc/mkb/mkb2_ghidra.h --use-core -o src/mkb.rs -- --target=powerpc-unknown-eabi
