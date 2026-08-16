.global custom_titlescreen_text_color

// Hooked at 0x8032ad0c GC
custom_titlescreen_text_color:

// Load original address of "PRESS START/PAUSE" string
lis r5, titlescreen_press_start_string@ha
ori r5, r5, titlescreen_press_start_string@l

// Calculate the index of the current character
sub r6, r30, r5
cmpwi r6, 5
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
lis r5, titlescreen_text_color_hook_resume@ha
ori r5, r5, titlescreen_text_color_hook_resume@l
mtctr r5
bctr
