;INPUT (1) / OUTPUT (1)
;Lê (ou grava) um byte na porta indicada por endereço imediato e coloca em REG8

;inp <a>, <8 bits>
;out <a>, <8 bits>
E0:
FETCH
ADDR_SEL_PC RD_MEMD WR_MAR PC_INC
EXTEND ;(EXT_INPUT_1 / EXT_OUTPUT_1)


;INPUT (2) / OUTPUT (2)
;Lê (ou grava) um byte na porta indicada por REG8 e coloca em REG8

;inp <a>, A
;out <a>, A
E1:
FETCH
RD_ACC WR_MAR
EXTEND ;(EXT_INPUT_1 / EXT_OUTPUT_1)

;inp <a>, B
;out <a>, B
E2:
FETCH
RD_B WR_MAR
EXTEND ;(EXT_INPUT_1 / EXT_OUTPUT_1)

;inp <a>, C
;out <a>, C
E3:
FETCH
RD_C WR_MAR
EXTEND ;(EXT_INPUT_1 / EXT_OUTPUT_1)

;inp <a>, D
;out <a>, D
E4:
FETCH
RD_D WR_MAR
EXTEND ;(EXT_INPUT_1 / EXT_OUTPUT_1)

;inp <a>, H
;out <a>, H
E5:
FETCH
RD_H WR_MAR
EXTEND ;(EXT_INPUT_1 / EXT_OUTPUT_1)

;inp <a>, L
;out <a>, L
E6:
FETCH
RD_L WR_MAR
EXTEND ;(EXT_INPUT_1 / EXT_OUTPUT_1)


;TODO: remover, instrução não pode ser implementada
;OUTPUT (3)
;Grava um byte imediato na porta indicada por endereço imediato


;EXT_INPUT_1
;Lê um byte na porta indicada por MAR e coloca em REG8

;inp A, <b>
1A4:
FETCH
ADDR_SEL_MAR RD_IOD WR_ACC
FINISH

;inp B, <b>
1A5:
FETCH
ADDR_SEL_MAR RD_IOD WR_B
FINISH

;inp C, <b>
1A6:
FETCH
ADDR_SEL_MAR RD_IOD WR_C
FINISH

;inp D, <b>
1A7:
FETCH
ADDR_SEL_MAR RD_IOD WR_D
FINISH

;inp H, <b>
1A8:
FETCH
ADDR_SEL_MAR RD_IOD WR_H
FINISH

;inp L, <b>
1A9:
FETCH
ADDR_SEL_MAR RD_IOD WR_L
FINISH


;EXT_OUTPUT_1
;Escreve um byte na porta indicada por MAR e coloca em REG8

;out A, <b>
1AA:
FETCH
ADDR_SEL_MAR RD_ACC WR_IOD
FINISH

;out B, <b>
1AB:
FETCH
ADDR_SEL_MAR RD_B WR_IOD
FINISH

;out C, <b>
1AC:
FETCH
ADDR_SEL_MAR RD_C WR_IOD
FINISH

;out D, <b>
1AD:
FETCH
ADDR_SEL_MAR RD_D WR_IOD
FINISH

;out H, <b>
1AE:
FETCH
ADDR_SEL_MAR RD_H WR_IOD
FINISH

;out L, <b>
1AF:
FETCH
ADDR_SEL_MAR RD_L WR_IOD
FINISH
