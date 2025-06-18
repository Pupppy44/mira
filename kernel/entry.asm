[bits 64]
[extern mk_entry]

section .text
global _start
_start:
    call mk_entry
    jmp $

section .rodata
global assets_start
assets_start:
    incbin "../build/assets.bin"
global assets_end
assets_end: