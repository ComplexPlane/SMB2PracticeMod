.global CustomTitleScreenTextColor

// Hooked at 0x8032ad0c GC
CustomTitleScreenTextColor:

// Load original address of "PRESS START/PAUSE" string
lis r5, 0x8047
ori r5, r5, 0xf4ec

// Calculate the index of the current character
sub r6, r30, r5
cmpwi r6, 9
bge Color2

Color1:
lis r6, 0xfff0
ori r6, r6, 0xf000
b End

Color2:
lis r6, 0xb15a
ori r6, r6, 0xff00

End:
stw r6, 0x8(sp)
lis r5, 0x8032
ori r5, r5, 0xad3c
mtctr r5
bctr
