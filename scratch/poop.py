#!/usr/bin/env python3

with open('poop.tsv') as f:
    for line in f:
        name, ghidra_start, ghidra_end, length = line.split('\t')[:4]
        ghidra_start = '0x' + ghidra_start + 'L'
        ghidra_end = '0x' + ghidra_end + 'L'
        length = length + 'L'

        ram_addr = 'RAMADDR'
        file_addr = 'FILEADDR'

        if 'uninitialized' in name:
            region_type = 'RegionType.BSS'
            file_addr = 'null'
        elif 'MAIN' in name or 'mkb2' in name:
            region_type = 'RegionType.INITIALIZED'
        else:
            region_type = 'RegionType.HARDWARE'
            ram_addr = ghidra_start
            file_addr = 'null'

        if 'MAIN' in name:
            ram_addr = ghidra_start

        print(f'new GameMemoryRegion({region_type}, "{name}", {length}, {ghidra_start}, {ram_addr}, {file_addr}),')
