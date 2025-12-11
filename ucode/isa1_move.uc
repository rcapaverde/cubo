;MOVE (1)
;Copia REG8X para REG8X
;mov <a>, <b>

;mov a, <b>
01:
FETCH
RD_ACC WR_MAR
EXTEND

;mov b, <b>
02:
FETCH
RD_B WR_MAR
EXTEND

;mov c, <b>
03:
FETCH
RD_C WR_MAR
EXTEND

;mov d, <b>
04:
FETCH
RD_D WR_MAR
EXTEND

;mov h, <b>
05:
FETCH
RD_H WR_MAR
EXTEND

;mov l, <b>
06:
FETCH
RD_L WR_MAR
EXTEND

;mov psw, <b>
07:
FETCH
RD_PSW WR_MAR
EXTEND

;mov <a>, a
180:
FETCH
RD_MAR WR_ACC
FINISH

;mov <a>, b
181:
FETCH
RD_MAR WR_B
FINISH

;mov <a>, c
182:
FETCH
RD_MAR WR_C
FINISH

;mov <a>, d
183:
FETCH
RD_MAR WR_D
FINISH

;mov <a>, h
184:
FETCH
RD_MAR WR_H
FINISH

;mov <a>, l
185:
FETCH
RD_MAR WR_L
FINISH

;mov <a>, psw
186:
FETCH
RD_MAR WR_PSW
FINISH


;MOVE (2)
;Copia REG16X para REG16X
;mov <a>, <b>

;mov BC, <b>
08:
FETCH
RD_C WR_MAR
RD_B WR_MAR
EXTEND

;mov HL, <b>
09:
FETCH
RD_L WR_MAR
RD_H WR_MAR
EXTEND

;mov SP, <b>
0A:
FETCH
RD_SP WR_MAR
RD_SP WR_MAR
EXTEND

;mov PC, <b>
0B:
FETCH
RD_PCL WR_MAR
RD_PCH WR_MAR
EXTEND

;mov <a>, BC
188:
FETCH
RD_MAR WR_C
RD_MAR WR_B
FINISH

;mov <a>, HL
189:
FETCH
RD_MAR WR_L
RD_MAR WR_H
FINISH

;mov <a>, SP
18A:
FETCH
RD_MAR WR_SP
RD_MAR WR_SP
FINISH

;mov <a>, PC
18B:
FETCH
RD_MAR WR_PCL
RD_MAR WR_PCH
FINISH


;MOVE (3)
;Copia acumulador para REG16 com extensão de sinal
;movsx <a>, A

;mov BC, A
1D:
FETCH
RD_ACC WR_C
ULA_SX
RD_ACC WR_B
RD_C WR_ACC
FINISH

;mov HL, A
1E:
FETCH
RD_ACC WR_L
ULA_SX
RD_ACC WR_H
RD_L WR_ACC
FINISH


;MOVE (4)
;Copia REG8 para REG16
;mov <a>, <b>

;mov A, <b>
10:
FETCH
RD_ACC WR_MAR
RD_ZERO WR_MAR
EXTEND ;ext_move_2

;mov B, <b>
11:
FETCH
RD_B WR_MAR
RD_ZERO WR_MAR
EXTEND ;ext_move_2

;mov C, <b>
12:
FETCH
RD_C WR_MAR
RD_ZERO WR_MAR
EXTEND ;ext_move_2

;mov D, <b>
13:
FETCH
RD_D WR_MAR
RD_ZERO WR_MAR
EXTEND ;ext_move_2

;mov H, <b>
14:
FETCH
RD_H WR_MAR
RD_ZERO WR_MAR
EXTEND ;ext_move_2

;mov L, <b>
15:
FETCH
RD_L WR_MAR
RD_ZERO WR_MAR
EXTEND ;ext_move_2
