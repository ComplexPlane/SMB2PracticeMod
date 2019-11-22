# Old stack

| Size                          | Offset |
| CARDMount work area: 0xa000   | 0x220  |
| CardFileInfo: 20              | 0x20c  |
| Small card file buffer: 0x200 | 0xc    |
| "rel" string                  | 0x8    |
| LR save                       | 0x4    |
| SP save                       | 0x0    |

0x72656c00

sum: 0xa214
with padding: 0xa218
with backchain / LR: 0xa220

# New stack

| Size                   | Offset | Size   | Alignment |
| ---------------------- | ------ | ------ | --------- |
| CARDMount work area    | 0xa220 | 0xa000 | 32        |
| Small card file buffer | 0x20   | 0x200  | 32        |
| CardFileInfo           | 0xc    | 0x14   |           |
| "rel" string           | 0x8    | 0x4    |           |
| LR Save                | 0x4    | 0x4    |           |
| SP Save                | 0x0    | 0x4    |           |
