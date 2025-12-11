.org 0x2000

start:
    ld a, 0     ;zera o valor do acumulador
    ld b, 0     ;número da porta

loop:
;    out b, a
;    inc a
;    jmp loop

.org 0xFFFE
.dw start
