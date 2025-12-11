
;NOP
00:
FETCH
FINISH

;não há PC_INC, paralisando o processador
;HALT
1F:
ADDR_SEL_PC RD_MEMD WR_INS
FINISH
