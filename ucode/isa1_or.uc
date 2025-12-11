;OR (1)
;OR bitwise 8 bits imediatos com ACC

;or A, <8 bits>
9E:
FETCH
ADDR_SEL_PC RD_MEMD ULA_OR PC_INC
FINISH


;OR (2)
;Bitwise OR 8 bits apontado por REG16 com ACC

;or A, [BC]
AB:
FETCH
RD_C WR_MAR
RD_B WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_OR
FINISH

;or A, [HL]
AC:
FETCH
RD_L WR_MAR
RD_H WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_OR
FINISH


;OR (3)
;Bitwise OR 8 bits apontados imediatos com ACC

;or A, [<addr>]
AD:
FETCH
ADDR_SEL_PC RD_MEMD WR_MAR PC_INC
ADDR_SEL_PC RD_MEMD WR_MAR PC_INC
ADDR_SEL_MAR RD_MEMD ULA_OR
FINISH


;OR (4)
;OR bitwise de REG8X com ACC

;or A, A
97:
FETCH
RD_ACC ULA_OR
FINISH

;or A, B
98:
FETCH
RD_B ULA_OR
FINISH

;or A, C
99:
FETCH
RD_C ULA_OR
FINISH

;or A, D
9A:
FETCH
RD_D ULA_OR
FINISH

;or A, H
9B:
FETCH
RD_H ULA_OR
FINISH

;or A, L
9C:
FETCH
RD_L ULA_OR
FINISH

;or A, PSW
9D:
FETCH
RD_PSW ULA_OR
FINISH
