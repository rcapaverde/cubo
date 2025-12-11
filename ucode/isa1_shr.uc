;SHIFT RIGHT CARRY (1)
;Deslocamento à direita de ACC, com carry

;shrc A
B6:
FETCH
RD_ACC ULA_SHR
FINISH


;SHIFT RIGHT CARRY (2)
;Deslocamento à direita do conteúdo apontado por REG16, resultado em ACC, com carry

;shrc [BC]
B7:
FETCH
RD_C WR_MAR
RD_B WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_SHR
FINISH

;shrc [HL]
B8:
FETCH
RD_L WR_MAR
RD_H WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_SHR
FINISH


;SHIFT RIGHT (1)
;Deslocamento à direita de ACC

;shr A
B9:
FETCH
ULA_CLRCF
RD_ACC ULA_SHR
FINISH


;SHIFT RIGHT (2)
;Deslocamento à direita do conteúdo apontado por REG16, resultado em ACC

;shr [<a>]
BA:
FETCH
ULA_CLRCF
RD_C WR_MAR
RD_B WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_SHR
FINISH

;shr [HL]
BB:
FETCH
ULA_CLRCF
RD_L WR_MAR
RD_H WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_SHR
FINISH


;ROTATE RIGHT (1)
;Rotação à direita de ACC

;ror A
C1:
FETCH
RD_ACC ULA_ROR
FINISH


;ROTATE RIGHT (2)
;Rotaciona à direita o conteúdo apontado por REG16, resultado em ACC

;ror [<BC>]
BE:
FETCH
RD_C WR_MAR
RD_B WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_ROR
FINISH

;ror [HL]
BF:
FETCH
RD_L WR_MAR
RD_H WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_ROR
FINISH
