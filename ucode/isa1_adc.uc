;ADC (1)
;Adiciona 8 bits imediatos a ACC, com carry
;adc A, <8 bits>

;adc A, <8 bits>
3F:
FETCH
ADDR_SEL_PC RD_MEMD ULA_ADD PC_INC
FINISH


;ADC (2)
;Adiciona 8 bits apontado por REG16 a ACC, com carry
;adc A, [<a>]

;adc A, [BC]
4C:
FETCH
RD_C WR_MAR
RD_B WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_ADD
FINISH

;adc A, [HL]
4D:
FETCH
RD_L WR_MAR
RD_H WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_ADD
FINISH


;ADC (3)
;Adiciona 8 bits apontados imediatos a ACC, com carry
;adc A, [<addr>]

;adc A, [16 bit]
4B:
FETCH
ADDR_SEL_PC RD_MEMD WR_MAR PC_INC
ADDR_SEL_PC RD_MEMD WR_MAR PC_INC
ADDR_SEL_MAR RD_MEMD ULA_ADD
FINISH


;ADC (4)
;Adiciona REG8 a ACC, com carry
;adc A, <a>

;adc A, A
70:
FETCH
RD_ACC ULA_ADD
FINISH

;adc A, B
71:
FETCH
RD_B ULA_ADD
FINISH

;adc A, C
72:
FETCH
RD_C ULA_ADD
FINISH

;adc A, D
73:
FETCH
RD_D ULA_ADD
FINISH

;adc A, H
74:
FETCH
RD_H ULA_ADD
FINISH

;adc A, L
75:
FETCH
RD_L ULA_ADD
FINISH
