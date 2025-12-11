;XOR (1)
;Bitwise XOR 8 bits imediatos com ACC

;xor A, <8 bits>
A7:
FETCH
ADDR_SEL_PC RD_MEMD ULA_XOR PC_INC
FINISH

;XOR (2)
;Bitwise XOR 8 bits apontado por REG16 com ACC

;xor A, [BC]
A8:
FETCH
RD_C WR_MAR
RD_B WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_XOR
FINISH

;xor A, [HL]
A9:
FETCH
RD_L WR_MAR
RD_H WR_MAR
ADDR_SEL_MAR RD_MEMD ULA_XOR
FINISH

;XOR (3)
;Bitwise XOR 8 bits apontados imediatos com ACC

;xor A, [<addr>]
AA:
FETCH
ADDR_SEL_PC RD_MEMD WR_MAR PC_INC
ADDR_SEL_PC RD_MEMD WR_MAR PC_INC 
ADDR_SEL_MAR RD_MEMD ULA_XOR
FINISH

;XOR (4)
;Bitwise XOR de REG8X com ACC

;xor A, A
A0:
FETCH
RD_ACC ULA_XOR
FINISH

;xor A, B
A1:
FETCH
RD_B ULA_XOR
FINISH

;xor A, C
A2:
FETCH
RD_C ULA_XOR
FINISH

;xor A, D
A3:
FETCH
RD_D ULA_XOR
FINISH

;xor A, H
A4:
FETCH
RD_H ULA_XOR
FINISH

;xor A, L
A5:
FETCH
RD_L ULA_XOR
FINISH

;xor A, PSW
A6:
FETCH
RD_PSW ULA_XOR
FINISH

