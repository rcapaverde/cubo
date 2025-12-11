;INC (1)
;Incrementa ACC (com efeito nos flags)

;inc A
6F:
FETCH
SET_SETCF
RD_ZERO ULA
SET_ADD
RD_ZERO ULA
FINISH


;INC (2)
;Incrementa REG16 (sem efeito nos flags)

;inc BC
69:
FETCH
WR_MAR
RD_C
RD_B
MAR_INC
WR_C
RD_MAR
WR_B
RD_MAR
FINISH

;inc HL
6A:
FETCH
WR_MAR
RD_L
RD_H 
MAR_INC
WR_L
RD_MAR
WR_H
RD_MAR
FINISH


;INC (3)
;Incrementa REG8 (sem efeito nos flags), menos ACC

;inc B
EA:
FETCH
WR_MAR
RD_B  
MAR_INC
WR_B
RD_MAR
FINISH

;inc C
EB:
FETCH
WR_MAR
RD_C  
MAR_INC
WR_C
RD_MAR
FINISH

;inc D
EC:
FETCH
WR_MAR
RD_D  
MAR_INC
WR_D
RD_MAR
FINISH

;inc H
ED:
FETCH
WR_MAR
RD_H  
MAR_INC
WR_H
RD_MAR
FINISH

;inc L
EE:
FETCH
WR_MAR
RD_L  
MAR_INC
WR_L
RD_MAR
FINISH


;DEC (1)
;Decrementa ACC (com efeito nos flags)

;dec A
7F:
FETCH
SET_CLRCF
RD_ZERO ULA
SET_SUB
RD_ZERO ULA
FINISH

;DEC (2)
;Decrementa REG16 (sem efeito nos flags)

;dec BC
F0:
FETCH
WR_MAR
RD_C
RD_B
MAR_DEC
WR_C
RD_MAR
WR_B
RD_MAR
FINISH

;dec HL
F1:
FETCH
WR_MAR
RD_L
RD_H 
MAR_DEC
WR_L
RD_MAR
WR_H
RD_MAR
FINISH


;DEC (3)
;Decrementa REG8 (sem efeito nos flags), menos ACC

;dec B
F2:
FETCH
WR_MAR
RD_B  
MAR_DEC
WR_B
RD_MAR
FINISH

;dec C
F3:
FETCH
WR_MAR
RD_C  
MAR_DEC
WR_C
RD_MAR
FINISH

;dec D
F4:
FETCH
WR_MAR
RD_D  
MAR_DEC
WR_D
RD_MAR
FINISH

;dec H
F5:
FETCH
WR_MAR
RD_H  
MAR_DEC
WR_H
RD_MAR
FINISH

;dec L
F6:
FETCH
WR_MAR
RD_L  
MAR_DEC
WR_L
RD_MAR
FINISH
