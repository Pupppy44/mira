[bits 64]
[extern mk_entry]

section .text
global _start
_start:
    call mk_entry
    jmp $