.org 0x2000

start:
    ld c, 0
    ld b, 0     ;número da porta
    jmp reload_a

loop:
    out a, b
    rol a
    jz reload_a
    jmp loop

reload_a:
    mov c, a
    shl a
    or a, 1
    mov a, c
    jmp loop
    
.org 0xFFFE
.dw start
