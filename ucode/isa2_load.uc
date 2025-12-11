;LOAD (1)
;Carrega 8 bits imediatos da memória para REG8X
;ld <a>, <8 bits>

;ld A, <8 bits>
16:
FETCH
WR_ACC
RD_MEMD_PC
PC_INC
FINISH

;ld B, <8 bits>
17:
FETCH
WR_B
RD_MEMD_PC
PC_INC
FINISH

;ld C, <8 bits>
18:
FETCH
WR_C
RD_MEMD_PC
PC_INC
FINISH

;ld D, <8 bits>
19:
FETCH
WR_D
RD_MEMD_PC
PC_INC
FINISH

;ld H, <8 bits>
1A:
FETCH
WR_H
RD_MEMD_PC
PC_INC
FINISH

;ld L, <8 bits>
1B:
FETCH
WR_L
RD_MEMD_PC
PC_INC
FINISH

;ld PSW, <8 bits>
1C:
FETCH
WR_PSW
RD_MEMD_PC
PC_INC
FINISH


;LOAD (2)
;Carrega 16 bits imediatos da memória para REG16X
;ld <a>, <16 bits>

;ld BC, <16 bits>
20:
FETCH
WR_C
RD_MEMD_PC
PC_INC
WR_B
RD_MEMD_PC
PC_INC
FINISH

;ld HL, <16 bits>
21:
FETCH
WR_L
RD_MEMD_PC
PC_INC
WR_H
RD_MEMD_PC
PC_INC
FINISH

;ld SP, <16 bits>
22:
FETCH
WR_SP
RD_MEMD_PC
PC_INC
RD_MEMD_PC
PC_INC
FINISH

;TODO: instrução igual a JMP <16 bits>
;ld PC, <16 bits>
23:
FETCH
WR_MAR
RD_MEMD_PC
PC_INC
RD_MEMD_PC
WR_PCL
RD_MAR
WR_PCH
RD_MAR
FINISH


;LOAD (3) / LOAD (4)
;Carrega 8 bits ou 16 bits apontados por REG16X para REG8X ou REG16X
;ld <a>, [<b>]

;ld a, [BC]
24:
FETCH
WR_MAR
RD_C
RD_B
EXTEND ;(EXT_LOAD_3 para REG8X ou EXT_LOAD_4 para REG16X)

;ld a, [HL]
25:
FETCH
WR_MAR
RD_L
RD_H
EXTEND ;(EXT_LOAD_3 para REG8X ou EXT_LOAD_4 para REG16X)

;ld a, [SP]
26:
FETCH
WR_MAR
RD_SP
RD_SP
EXTEND ;(EXT_LOAD_3 para REG8X ou EXT_LOAD_4 para REG16X)

;ld a, [PC]
27:
FETCH
WR_MAR
RD_PCL
RD_PCH
EXTEND ;(EXT_LOAD_3 para REG8X ou EXT_LOAD_4 para REG16X)

;ld A, [b]
190:
FETCH
WR_ACC
RD_MEMD_MAR
FINISH

;ld B, [b]
191:
FETCH
WR_B
RD_MEMD_MAR
FINISH

;ld C, [b]
192:
FETCH
WR_C
RD_MEMD_MAR
FINISH

;ld D, [b]
193:
FETCH
WR_D
RD_MEMD_MAR
FINISH

;ld H, [b]
194:
FETCH
WR_H
RD_MEMD_MAR
FINISH

;ld L, [b]
195:
FETCH
WR_L
RD_MEMD_MAR
FINISH

;ld PSW, [b]
196:
FETCH
WR_PSW
RD_MEMD_MAR
FINISH


;LOAD (4)
;Carrega 16 bits apontados por REG16X para REG16X
;ld <a>, [<b>]

;ld BC, [b]
18C:
FETCH
WR_C
RD_MEMD_MAR
MAR_INC
WR_B
RD_MEMD_MAR
FINISH

;ld HL, [b]
18D:
FETCH
WR_L
RD_MEMD_MAR
MAR_INC
WR_H
RD_MEMD_MAR
FINISH

;ld SP, [b]
18E:
FETCH
WR_SP
RD_MEMD_MAR
MAR_INC
WR_SP
RD_MEMD_MAR
FINISH

;ld PC, [b]
18F:
FETCH
WR_PCL
RD_MEMD_MAR
MAR_INC
WR_PCH
RD_MEMD_MAR
FINISH


;LOAD (5)
;Carrega 8 bits apontados imediatamente para REG8X
;ld <a>, [<addr>]

;ld A, [16 bit]
30:
FETCH
WR_MAR
RD_MEMD_PC
PC_INC
RD_MEMD_PC
PC_INC
WR_ACC
RD_MEMD_MAR
FINISH

;ld B, [16 bit]
31:
FETCH
WR_MAR
RD_MEMD_PC
PC_INC
RD_MEMD_PC
PC_INC
WR_B
RD_MEMD_MAR
FINISH

;ld C, [16 bit]
32:
FETCH
WR_MAR
RD_MEMD_PC
PC_INC
RD_MEMD_PC
PC_INC
WR_C
RD_MEMD_MAR
FINISH

;ld D, [16 bit]
33:
FETCH
WR_MAR
RD_MEMD_PC
PC_INC
RD_MEMD_PC
PC_INC
WR_D
RD_MEMD_MAR
FINISH

;ld H, [16 bit]
34:
FETCH
WR_MAR
RD_MEMD_PC
PC_INC
RD_MEMD_PC
PC_INC
WR_H
RD_MEMD_MAR
FINISH

;ld L, [16 bit]
35:
FETCH
WR_MAR
RD_MEMD_PC
PC_INC
RD_MEMD_PC
PC_INC
WR_L
RD_MEMD_MAR
FINISH

;ld PSW, [16 bit]
36:
FETCH
WR_MAR
RD_MEMD_PC
PC_INC
RD_MEMD_PC
PC_INC
WR_PSW
RD_MEMD_MAR
FINISH


;LOAD (6)
;Carrega 16 bits apontados imediatamente para REG16X
;ld <a>, [<addr>]

;ld BC, [16 bit]
0C:
FETCH
WR_MAR
RD_MEMD_PC
PC_INC
RD_MEMD_PC
PC_INC
WR_C
RD_MEMD_MAR
MAR_INC
WR_B
RD_MEMD_MAR
FINISH

;ld HL, [16 bit]
0D:
FETCH
WR_MAR
RD_MEMD_PC
PC_INC
RD_MEMD_PC
PC_INC
WR_L
RD_MEMD_MAR
MAR_INC
WR_H
RD_MEMD_MAR
FINISH

;ld SP, [16 bit]
0E:
FETCH
WR_MAR
RD_MEMD_PC
PC_INC
RD_MEMD_PC
PC_INC
WR_SP
RD_MEMD_MAR
MAR_INC
RD_MEMD_MAR
FINISH

;ld PC, [16 bit]
0F:
FETCH
WR_MAR
RD_MEMD_PC
PC_INC
RD_MEMD_PC
PC_INC
WR_PCL
RD_MEMD_MAR
MAR_INC
WR_PCH
RD_MEMD_MAR
FINISH
