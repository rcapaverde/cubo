.segment text

; HL = H * L
; multiplica H por L, colocando o resultado em HL
; os flags ZERO e NEGATIVE s√£o afetados; CARRY e OVERFLOW s√£o desconhecidos
; todos os demais registradores s√£o preservados
cubo_mul_8:
    psh a       ; preserva os registradores usados como tempor√°rios
    psh d
    psh bc

    mov a, h    ; multiplicando vai para BC (com extens√£o de sinal)
    movsx bc, a 
    mov a, l    ; multiplicador vai para o acumulador
    ld d, 8     ; n√∫mero de bits a multiplicar

    ld hl, 0    ; inicializa HL

_loop:
    shr a       ; desloca o multiplicador para a direita, de forma a 
    jnc _nsum   ; colocar o bit menos significativo no CARRY
                ; se este bit for 1, soma o multiplicando ao resultado

    swp a, l    ; soma BC em HL
    add a, c
    swp a, l
    swp a, h
    adc a, b
    swp a, h
    jmp _proximo

_nsum:
    jz _flags   ; se o multiplicar for zero, encerra a operaÁ„o

_proximo:
    swp a, c    ; desloca BC um bit a esquerda (ou seja, multiplica ele por 2)
    shl a
    swp a, c
    swp a, b
    shlc a
    swp a, b

    dec d       ; decrementa a quantidade de bits a multiplicar
    swp a, c
    cmp a, 0
    swp a, c
    jnz _loop   ; repete o deslocamento-soma atÈ completar os bits do multiplicador

_flags:
    mov a, h    ; testa se o byte mais significativo È diferente de zero
    cmp a, 0
    jnz _end    ; se for, preserva os flags ZERO e NEGATIVE obtidos do cmp acima
    mov a, l    ; se o MSB for zero, tem que testar o LSB
    cmp a, 0
    swp a, psw
    and a, 0xF7 ; mas neste caso, ignora o NEGATIVE, colocando ele sempre como zero
    swp a, psw

_end:
    pop bc
    pop d
    pop a
    ret
