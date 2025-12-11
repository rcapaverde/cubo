.segment text

; HL = HL - BC
; recebe dois números de 16 bits em HL e BC, subtrai e coloca o resultado em HL
; os flags CARRY, OVERFLOW, ZERO e NEGATIVE são afetados
; registradores A, B, C e D são preservados
cubo_sub_16:
    psh a           ; salva os registradores A e D
    psh d
    
    mov a, l        ; subtrai os bytes de menor ordem (L e C)
    sub a, c        
    mov l, a        ; resultado volta para L    
    mov d, psw      ; salva o flag ZERO, que será combinado com ele mesmo na segunda soma 

    mov a, h        ; subtrai os bytes de maior ordem (H e B)
    sbb a, b        ; desta vez, a subtração considera o CARRY da operação anterior
    mov h, a        ; resultado volta para H
    
    mov a, psw      ; combina os flags ZERO da primeira com a segunda soma 
    swp a, d
    or a, 0xFE
    and a, d

    pop d           ; restaura o conteúdo dos registradores A e D
    pop a

    ret
