FETCH:
RD_MEMD_PC
PC_INC

DEFAULT:
FETCH
FINISH

;reset
100:
NOP
WR_PCL CONST_RSTL
RD_MEMD_CONST
WR_PCH CONST_RSTH
RD_MEMD_CONST
FINISH

;interrupt handler
1FF:
WR_MEMD_SP
RD_PCH
SP_DEC
RD_PCL
SP_DEC
WR_PCL CONST_INTL
RD_MEMD_CONST
WR_PCH CONST_INTH
RD_MEMD_CONST
CLEAR_INTR
SET_CLRIF
RD_ZERO ULA
FINISH

.inc isa2_other.uc
.inc isa2_flags.uc
.inc isa2_jump.uc
.inc isa2_inc_dec.uc
.inc isa2_in_out.uc
.inc isa2_load.uc
