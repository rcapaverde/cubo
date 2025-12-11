;SBB (1)
;Subtrai 8 bits imediatos de ACC, com borrow

;sbb A, <8 bits>
6B:
FETCH
ADDR_SEL_PC RD_MEMD ULA_SUB PC_INC
FINISH

;SBB (2)
;Subtrai 8 bits apontado por REG16 de ACC, com borrow

;sbb A, [BC]
6C:
FETCH
RD_C WR_MAR
RD_B WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_SUB
FINISH

;sbb A, [HL]
6D:
FETCH
RD_L WR_MAR
RD_H WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_SUB
FINISH


;SBB (3)
;Subtrai 8 bits apontados imediatos de ACC, com borrow

;sbb A, [<addr>]
6E:
FETCH
ADDR_SEL_PC RD_MEMD WR_MAR PC_INC
ADDR_SEL_PC RD_MEMD WR_MAR PC_INC
ADDR_SEL_MAR RD_MEMD ULA_SUB
FINISH


;SBB (4)
;Subtrai REG8 de ACC, com borrow

;sbb A, A
80:
FETCH
RD_ACC ULA_SUB
FINISH

;sbb A, B
81:
FETCH
RD_B ULA_SUB
FINISH

;sbb A, C
82:
FETCH
RD_C ULA_SUB
FINISH

;sbb A, D
83:
FETCH
RD_D ULA_SUB
FINISH

;sbb A, H
84:
FETCH
RD_H ULA_SUB
FINISH

;sbb A, L
85:
FETCH
RD_L ULA_SUB
FINISH
