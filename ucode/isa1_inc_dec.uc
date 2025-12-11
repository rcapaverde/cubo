;INC (1)
;Incrementa ACC (com efeito nos flags)

;inc A
6F:
FETCH
ULA_SETCF
RD_ZERO ULA_ADD
FINISH


;INC (2)
;Incrementa REG16 (sem efeito nos flags)

;inc BC
69:
FETCH
RD_C WR_MAR
RD_B WR_MAR MAR_INC
RD_MAR WR_C
RD_MAR WR_B
FINISH

;inc HL
6A:
FETCH
RD_L WR_MAR
RD_H WR_MAR MAR_INC
RD_MAR WR_L
RD_MAR WR_H
FINISH


;INC (3)
;Incrementa REG8 (sem efeito nos flags), menos ACC

;inc B
EA:
FETCH
RD_B WR_MAR MAR_INC
RD_MAR WR_B
FINISH

;inc C
EB:
FETCH
RD_C WR_MAR MAR_INC
RD_MAR WR_C
FINISH

;inc D
EC:
FETCH
RD_D WR_MAR MAR_INC
RD_MAR WR_D
FINISH

;inc H
ED:
FETCH
RD_H WR_MAR MAR_INC
RD_MAR WR_H
FINISH

;inc L
EE:
FETCH
RD_L WR_MAR MAR_INC
RD_MAR WR_L
FINISH


;DEC (1)
;Decrementa ACC (com efeito nos flags)

;dec A
7F:
FETCH
ULA_CLRCF
RD_ZERO ULA_SUB
FINISH

;DEC (2)
;Decrementa REG16 (sem efeito nos flags)

;dec BC
F0:
FETCH
RD_C WR_MAR
RD_B WR_MAR MAR_DEC
RD_MAR WR_C
RD_MAR WR_B
FINISH

;dec HL
F1:
FETCH
RD_L WR_MAR
RD_H WR_MAR MAR_DEC
RD_MAR WR_L
RD_MAR WR_H
FINISH


;DEC (3)
;Decrementa REG8 (sem efeito nos flags), menos ACC

;dec B
F2:
FETCH
RD_B WR_MAR MAR_DEC
RD_MAR WR_B
FINISH

;dec C
F3:
FETCH
RD_C WR_MAR MAR_DEC
RD_MAR WR_C
FINISH

;dec D
F4:
FETCH
RD_D WR_MAR MAR_DEC
RD_MAR WR_D
FINISH

;dec H
F5:
FETCH
RD_H WR_MAR MAR_DEC
RD_MAR WR_H
FINISH

;dec L
F6:
FETCH
RD_L WR_MAR MAR_DEC
RD_MAR WR_L
FINISH
