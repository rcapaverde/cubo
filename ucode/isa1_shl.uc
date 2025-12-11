;SHIFT LEFT CARRY (1)
;Deslocamento à esquerda de ACC, com carry

;shlc A
B0:
FETCH
RD_ACC ULA_SHL
FINISH


;SHIFT LEFT CARRY (2)
;Deslocamento à esquerda do conteúdo apontado por REG16, resultado em ACC, com carry

;shlc [BC]
B1:
FETCH
RD_C WR_MAR
RD_B WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_SHL
FINISH

;shlc [HL]
B2:
FETCH
RD_L WR_MAR
RD_H WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_SHL
FINISH


;SHIFT LEFT (1)
;Deslocamento à esquerda de ACC

;shl A
B3:
FETCH
ULA_CLRCF
RD_ACC ULA_SHL
FINISH


;SHIFT LEFT (2)
;Deslocamento à esquerda do conteúdo apontado por REG16, resultado em ACC

;shl [BC]
B4:
FETCH
ULA_CLRCF
RD_C WR_MAR
RD_B WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_SHL
FINISH

;shl [HL]
B5:
FETCH
ULA_CLRCF
RD_L WR_MAR
RD_H WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_SHL
FINISH


;ROTATE LEFT (1)
;Rotação à esquerda de ACC

;rol A
C0:
FETCH
RD_ACC ULA_ROL
FINISH


;ROTATE LEFT (2)
;Rotaciona à esquerda o conteúdo apontado por REG16, resultado em ACC

;rol [BC]
BC:
FETCH
RD_C WR_MAR
RD_B WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_ROL
FINISH

;rol [HL]
BD:
FETCH
RD_L WR_MAR
RD_H WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_ROL
FINISH
