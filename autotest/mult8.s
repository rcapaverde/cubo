.org 0xfffe
.dw start

.segment text
.org 0x2000

start:

ld h, 50
ld l, 10
jsr cubo_mul_8
hlt
