# C Port

## Previous Attempt

if in doubt, reference a C port of an older branch here: /Users/alex/Documents/repos/2019/pracmod-tmp

## Naming Conventions

Non-static functions should use the naming convention `ModuleName_FunctionName`.

Examples:

`card::write_file()` -> `Card_WriteFile()`
`modlink::get()` -> `ModLink_Get()`

Static functions should continue to use `snake_case`.

Static variables should use `snake_case` and be prefixed with `s_`, as before.

Local variables should use `snake_case`, as before.

Structs and enums in headers should use `ModuleName_StructName` / `ModuleName_EnumName`. Enum
variants should be like `ModuleName_EnumName_VariantName`.

Sometimes, shorter module names are OK. For example, `SS_` for `libsavest` and `Card_` for `cardio`.
Use your best discretion.

## Makefile

See pracmod-tmp's Makefile for any C-specific changes.
