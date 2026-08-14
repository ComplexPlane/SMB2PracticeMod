#!/usr/bin/env bash
set -euo pipefail
trap 's=$?; echo >&2 "$0: Error on line "$LINENO": $BASH_COMMAND"; exit $s' ERR

OUT_DIR="$HOME/Library/Application Support/Dolphin/GC/USA/Card B"

make -j8
mkdir -p "$OUT_DIR"
cp SMB2PracticeMod.gci "$OUT_DIR"
