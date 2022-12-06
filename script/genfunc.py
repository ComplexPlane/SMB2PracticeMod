#!/usr/bin/env python3

import sys


def printe(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


if __name__ == '__main__':
    if len(sys.argv) < 2:
        printe('Not enough arguments')
        sys.exit(1)

    arg1 = sys.argv[1]
    if arg1[0] == 'r':
        nregs = 32 - int(arg1[1:])
    else:
        nregs = int(arg1)

    if not 0 <= nregs <= 18:
        printe(f'Invalid number of non-volatile registers: {nregs}')
        sys.exit(1)

    frame_size = 4 * (2 + nregs)
    # Add padding to make frame doubleword-aligned
    if frame_size % 8 != 0:
        frame_size += 4

    print('# Prologue')

    print(f'stwu r1, -{hex(frame_size)} (r1)')
    print('mflr r0')
    print(f'stw r0, {hex(frame_size + 4)} (r1)')

    for reg_idx in range(nregs):
        reg_num = 31 - reg_idx
        save_offset = frame_size - ((reg_idx + 1) * 4)
        print(f'stw r{reg_num}, {hex(save_offset)} (r1)')

    print()

    print('# Epilogue')

    for reg_idx in range(nregs):
        reg_num = 31 - reg_idx
        save_offset = frame_size - ((reg_idx + 1) * 4)
        print(f'lwz r{reg_num}, {hex(save_offset)} (r1)')

    print(f'lwz r0, {hex(frame_size + 4)} (r1)')
    print('mtlr r0')
    print(f'addi r1, r1, {hex(frame_size)}')
    print(f'blr')
