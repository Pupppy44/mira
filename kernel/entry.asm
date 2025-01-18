[bits 64]
[extern mk_main]

section .text
global _start
_start:
    call mk_main
    jmp $