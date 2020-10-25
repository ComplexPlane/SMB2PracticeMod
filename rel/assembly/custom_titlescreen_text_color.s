.global custom_titlescreen_text_color

// Hooked at 0x8032ad0c GC
custom_titlescreen_text_color:

// Load original address of "PRESS START/PAUSE" string
lis r5, 0x8047
ori r5, r5, 0xf4ec

// Calculate the index of the current character
sub r6, r30, r5
cmpwi r6, 9
bge color2

color1:
lis r6, 0xfff0
ori r6, r6, 0xf000
b end

color2:
lis r6, 0xb15a
ori r6, r6, 0xff00

end:
stw r6, 0x8(sp)
lis r5, 0x8032
ori r5, r5, 0xad3c
mtctr r5
bctr
