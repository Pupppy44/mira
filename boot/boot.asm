; Set Program Origin
[org 0x7C00]

; Skip the data below
jmp begin_real

; Kernel Size
kernel_size db 0

; 16-bit Mode
begin_real:

[bits 16]

; Initialize the base pointer and the stack pointer
mov bp, 0x0500
mov sp, bp

; Save the boot drive ID
mov byte[boot_drive], dl

; Load the second sector (skip printing)
mov bx, 0x0002

; Load boot + kernel sectors
mov cx, [kernel_size]
add cx, 2
mov dx, 0x7E00

; Load new sectors
call load_bios

; Elevate CPU to 32-bit mode
call elevate_bios

; Infinite Loop (to halt execution here)
bootsector_hold:
jmp $               ; Infinite loop

; INCLUDES
%include "16-bit/load.asm"
%include "16-bit/gdt.asm"
%include "16-bit/elevate.asm"

; DATA STORAGE AREA

; Boot drive storage
boot_drive:                     db 0x00

; Pad boot sector for magic number
times 510 - ($ - $$) db 0x00

; Magic number
dw 0xAA55

; BEGIN SECOND SECTOR. THIS ONE CONTAINS 32-BIT CODE ONLY

bootsector_extended:
begin_protected:

[bits 32]

; Initialize the page table
call init_pt_protected

call elevate_protected

jmp $       ; Infinite Loop

; INCLUDE protected-mode functions
%include "32-bit/pdt.asm"
%include "32-bit/gdt.asm"
%include "32-bit/elevate.asm"

; Fill with zeros to the end of the sector
times 512 - ($ - bootsector_extended) db 0x00
begin_long_mode:

[bits 64]

; Jump directly to kernel in long mode
call kernel_start

jmp $       ; Infinite loop

kernel_start:                   equ 0x8200              ; Kernel is at 1MB

times 512 - ($ - begin_long_mode) db 0x00