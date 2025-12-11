;INPUT (1) / OUTPUT (1)
;Lê (ou grava) um byte na porta indicada por endereço imediato e coloca em REG8

;inp <a>, <8 bits>
;out <a>, <8 bits>
E0:
FETCH
WR_MAR
RD_MEMD_PC
PC_INC
EXTEND ;(EXT_INPUT_1 / EXT_OUTPUT_1)


;INPUT (2) / OUTPUT (2)
;Lê (ou grava) um byte na porta indicada por REG8 e coloca em REG8

;inp <a>, A
;out <a>, A
E1:
FETCH
WR_MAR
RD_ACC
EXTEND ;(EXT_INPUT_1 / EXT_OUTPUT_1)

;inp <a>, B
;out <a>, B
E2:
FETCH
WR_MAR
RD_B
EXTEND ;(EXT_INPUT_1 / EXT_OUTPUT_1)

;inp <a>, C
;out <a>, C
E3:
FETCH
WR_MAR
RD_C
EXTEND ;(EXT_INPUT_1 / EXT_OUTPUT_1)

;inp <a>, D
;out <a>, D
E4:
FETCH
WR_MAR
RD_D
EXTEND ;(EXT_INPUT_1 / EXT_OUTPUT_1)

;inp <a>, H
;out <a>, H
E5:
FETCH
WR_MAR
RD_H
EXTEND ;(EXT_INPUT_1 / EXT_OUTPUT_1)

;inp <a>, L
;out <a>, L
E6:
FETCH
WR_MAR
RD_L
EXTEND ;(EXT_INPUT_1 / EXT_OUTPUT_1)


;TODO: remover, instrução não pode ser implementada
;OUTPUT (3)
;Grava um byte imediato na porta indicada por endereço imediato


;EXT_INPUT_1
;Lê um byte na porta indicada por MAR e coloca em REG8

;inp A, <b>
1A4:
FETCH
WR_ACC
RD_IOD_MAR
FINISH

;inp B, <b>
1A5:
FETCH
WR_B
RD_IOD_MAR
FINISH

;inp C, <b>
1A6:
FETCH
WR_C
RD_IOD_MAR
FINISH

;inp D, <b>
1A7:
FETCH
WR_D
RD_IOD_MAR
FINISH

;inp H, <b>
1A8:
FETCH
WR_H
RD_IOD_MAR
FINISH

;inp L, <b>
1A9:
FETCH
WR_L
RD_IOD_MAR
FINISH


;EXT_OUTPUT_1
;Escreve um byte na porta indicada por MAR e coloca em REG8

;out A, <b>
1AA:
FETCH
WR_IOD_MAR
RD_ACC
FINISH

;out B, <b>
1AB:
FETCH
WR_IOD_MAR
RD_B
FINISH

;out C, <b>
1AC:
FETCH
WR_IOD_MAR
RD_C
FINISH

;out D, <b>
1AD:
FETCH
WR_IOD_MAR
RD_D
FINISH

;out H, <b>
1AE:
FETCH
WR_IOD_MAR
RD_H
FINISH

;out L, <b>
1AF:
FETCH
WR_IOD_MAR
RD_L
FINISH
