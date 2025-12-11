.ORG 0x2000

start:
    LD A, 22
    LD B, 5
    ADD A, B
    OUT A, 0

.ORG 0xFFFE
.dw start