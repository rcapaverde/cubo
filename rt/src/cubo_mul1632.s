.segment text

; [HL] = [HL] * [BC]
; multiplica o valor apontado por  por L, colocando o resultado em HL
; os flags ZERO e NEGATIVE são afetados; CARRY e OVERFLOW são desconhecidos
; todos os demais registradores são preservados
cubo_mul_8:
    psh a       ; preserva os registradores usados como temporários
    psh d
    psh bc

    mov a, h    ; multiplicando vai para BC (com extensão de sinal)
    movsx bc, a 
    mov a, l    ; multiplicador vai para o acumulador
    ld d, 8     ; número de bits a multiplicar

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
    jz _flags   ; se o multiplicar for zero, encerra a operação

_proximo:
    swp a, c    ; desloca BC um bit a esquerda (ou seja, multiplica ele por 2)
    shl a
    swp a, c
    swp a, b
    shlc a
    swp a, b

    dec d       ; decrementa a quantidade de bits a multiplicar
    cmp c, 0
    jnz _loop   ; repete o deslocamento-soma até completar os bits do multiplicador

_flags:
    mov a, h    ; testa se o byte mais significativo é diferente de zero
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
