FETCH:
ADDR_SEL_PC RD_MEMD WR_INS PC_INC

DEFAULT:
FETCH
FINISH

;reset
100:
NOP
ADDR_SEL_RSTL RD_MEMD WR_PCL
ADDR_SEL_RSTH RD_MEMD WR_PCH
RD_ZERO WR_PSW CLEAR_INTR
FINISH

;interrupt handler
1FF:
ADDR_SEL_SP RD_PCH WR_MEMD SP_DEC
ADDR_SEL_INTH RD_MEMD WR_PCH
ADDR_SEL_SP RD_PCL WR_MEMD SP_DEC
ADDR_SEL_INTL RD_MEMD WR_PCL
CLEAR_INTR ULA_CLRIF
FINISH

.inc isa1_load.uc
.inc isa1_move.uc
.inc isa1_other.uc
.inc isa1_store.uc
.inc isa1_push.uc
.inc isa1_pop.uc
.inc isa1_adc.uc
.inc isa1_add.uc
.inc isa1_jump.uc
.inc isa1_in_out.uc
.inc isa1_jsr.uc
.inc isa1_inc_dec.uc
.inc isa1_sbb.uc
.inc isa1_sub.uc
.inc isa1_and.uc
.inc isa1_or.uc
.inc isa1_xor.uc
.inc isa1_swap.uc
.inc isa1_not.uc
.inc isa1_shl.uc
.inc isa1_shr.uc
.inc isa1_flags.uc
.inc isa1_negative.uc
.inc isa1_compare.uc
