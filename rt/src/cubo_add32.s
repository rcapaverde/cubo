.segment text

; [HL] = [HL] + [BC]
; recebe dois números de 32 bits apontados por HL e BC, 
; soma e coloca o resultado no endereço apontado por HL
; os flags CARRY, OVERFLOW, ZERO e NEGATIVE são afetados
; registradores A, B, C, D, H e L são preservados
cubo_add_32:
    psh a           ; salva os registradores A, D, BC e HL
    psh d
    psh bc
    psh hl
    
    psh 0           ; conteúdo do flag ZERO
    mov d, 4        ; quantidade de bytes a serem operados

_loop:    
    ld a, [hl]      ; soma os bytes apontados por HL e BC
    add a, [bc]
    mov [hl], a     ; resultado volta para [HL]

    pop a           ; combina o flag ZERO da operação atual com as anteriores ...
    or a, 0xFE
    and a, psw
    psh a           ; ... e devolve o resultado para a pilha

    inc hl          ; avança os ponteiros para os próximos bytes
    inc bc          

    dec d           ; decrementa o contador com a quantidade de bytes ainda a serem somados
    cmp d, 0
    jnz _loop

; conclui e retorna
    pop psw         ; restaura os flags da operação

    psh hl          ; restaura os registradores
    psh bc
    psh d
    psh a

    ret
