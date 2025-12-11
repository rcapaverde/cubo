;NOT (1)
;Bitwise NOT de 8 bits apontados imediatos, resultado em ACC

;not [<addr>]
AE:
FETCH
ADDR_SEL_PC RD_MEMD WR_MAR PC_INC
ADDR_SEL_PC RD_MEMD WR_MAR PC_INC
ADDR_SEL_MAR RD_MEMD ULA_NOT
FINISH


;NOT (2)
;Bitwise NOT de ACC

;not A
AF:
FETCH
RD_ACC ULA_NOT
FINISH


;NOT (3)
;Bitwise NOT de 8 bits apontados por REG16, resultado em ACC

;not [BC]
C8:
FETCH
RD_C WR_MAR
RD_B WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_NOT
FINISH

;not [HL]
C9:
FETCH
RD_L WR_MAR
RD_H WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_NOT
FINISH
