;CLEAR CF
;Limpa o carry flag

;clc
C2:
FETCH
SET_CLRCF
RD_ZERO ULA
FINISH


;SET CF
;Liga o carry flag

;stc
C3:
FETCH
SET_SETCF
RD_ZERO ULA
FINISH

;ENABLE INT
;habilita interrupções
;interrupções solicitadas enquanto este flag estava desabilitado serão atendidas imediatamente

;ei
4A:
FETCH
SET_SETIF
RD_ZERO ULA
FINISH


;DISBLE INT
;desabilita interrupções

;di
EF:
FETCH
SET_CLRIF
RD_ZERO ULA
FINISH
