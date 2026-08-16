.global full_debug_text_color

// Hooked at 0x802aeca4 GC
full_debug_text_color:

lis r5, debug_text_color@ha
lwz r6, debug_text_color@l(r5)
cmpwi r6, 0
beq end
mr r4, r5

end:
li r3, 1 // Overwritten instruction
lis r5, debug_text_color_hook_resume@ha
ori r5, r5, debug_text_color_hook_resume@l
mtctr r5
bctr
