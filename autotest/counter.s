.org 0x2000

start:
    mov a, zero     ;zera o valor do acumulador         ; 10 01
    mov b, zero     ;número da porta                    ; 10 02

loop:
    out b, a        ;exibe o valor atual                ; 8C 12
    stc             ;incrementa o acumulador            ; 7C 00
    adc a, zero                                         ; 50 01
    jmp loop                                            ; 4E 8C 04 20

.org 0xFFFE
.dw start
