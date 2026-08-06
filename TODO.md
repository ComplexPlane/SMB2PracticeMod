# TODO

## C port

<!-- Ghidra plugin C-compatible header export -->
<!-- elf2rel: support optional symbol prefix -->
<!-- Makefile: support compiling some files as C -->
<!-- Makefile: stricter compiler flags -->
<!-- Single C test file -->

## Cleanup tasks

- Since we're using C23, avoid func(void) argument lists, just use func()
- Enums should generally have the capitalization/style of `EnumName_TheVariant`.
