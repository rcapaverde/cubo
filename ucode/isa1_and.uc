;AND (1)
;AND bitwise 8 bits imediatos com ACC

;and A, <8 bits>
8D:
FETCH
ADDR_SEL_PC RD_MEMD ULA_AND PC_INC
FINISH


;AND (2)
;AND bitwise 8 bits apontado por REG16 com ACC

;and A, [BC]
8E:
FETCH
RD_C WR_MAR
RD_B WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_AND
FINISH

;and A, [HL]
8F:
FETCH
RD_L WR_MAR
RD_H WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_AND
FINISH

;AND (3)
;AND bitwise 8 bits apontados imediatos com ACC

;and A, [<addr>]
9F:
FETCH
ADDR_SEL_PC RD_MEMD WR_MAR PC_INC
ADDR_SEL_PC RD_MEMD WR_MAR PC_INC
ADDR_SEL_MAR RD_MEMD ULA_AND
FINISH


;AND (4)
;AND bitwise de REG8X com ACC

;and A, A
90:
FETCH
RD_ACC ULA_AND
FINISH

;and A, B
91:
FETCH
RD_B ULA_AND
FINISH

;and A, C
92:
FETCH
RD_C ULA_AND
FINISH

;and A, D
93:
FETCH
RD_D ULA_AND
FINISH

;and A, H
94:
FETCH
RD_H ULA_AND
FINISH

;and A, L
95:
FETCH
RD_L ULA_AND
FINISH

;and A, PSW
96:
FETCH
RD_PSW ULA_AND
FINISH
