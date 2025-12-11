;NEGATIVE (1)
;Complemente de 2 de 8 bits apontados imediatos, resultado em ACC

;neg [<addr>]
C4:
FETCH
ADDR_SEL_PC RD_MEMD WR_MAR PC_INC
ADDR_SEL_PC RD_MEMD WR_MAR PC_INC
ADDR_SEL_MAR RD_MEMD ULA_NOT
ULA_SETCF
RD_ZERO ULA_ADD
FINISH


;NEGATIVE (2)
;Complemento de 2 de ACC

;neg A
C5:
FETCH
RD_ACC ULA_NOT
ULA_SETCF
RD_ZERO ULA_ADD
FINISH


;NEGATIVE (3)
;Complemento de 2 de 8 bits apontados por REG16, resultado em ACC

;neg [<BC>]
C6:
RD_C WR_MAR
RD_B WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_NOT
ULA_SETCF
RD_ZERO ULA_ADD
FINISH

;neg [<HL>]
C7:
RD_L WR_MAR
RD_H WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_NOT
ULA_SETCF
RD_ZERO ULA_ADD
FINISH
