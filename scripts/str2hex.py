#!/usr/bin/env python3

import sys


if __name__ == '__main__':
    string = sys.argv[1] + '\0'
    mod = len(string) % 4
    if mod != 0:
        string += ('\0' * (4 - mod))

    for i in range(0, len(string), 4):
        word = '0x'
        for j in range(4):
            char_hex = hex(ord(string[i+j]))[2:]
            if len(char_hex) == 1:
                word += '0' + char_hex
            else:
                word += char_hex
        print(word)
