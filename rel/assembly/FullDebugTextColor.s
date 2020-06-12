.global FullDebugTextColor

// Hooked at 0x802aeca4 GC
FullDebugTextColor:

lis r5, debugTextColor@h
ori r5, r5, debugTextColor@l
lwz r6, 0x0(r5)
cmpwi r6, 0
beq end
mr r4, r5

end:
li r3, 1 // Overwritten instruction
lis r5, 0x802a
ori r5, r5, 0xeca8
mtctr r5
bctr
