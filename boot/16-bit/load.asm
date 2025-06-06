;
; Long Mode
;
; load.asm
;

[bits 16]

; Define load_sectors
;   Sector start point in bx
;   Number of sectors to read in cx
;   Destination address in dx
load_bios:
    ; Save the registers
    push ax
    push bx
    push cx
    push dx

    ; Save the number of registers to load for later
    push cx

    ; For the ATA Read bios utility, the value of ah must be 0x02
    ; See the BIOS article from Chapter 1.2 for more info
    mov ah, 0x02

    ; The number of sectors to read must be in al, but our function
    ; takes it in cl
    mov al, cl

    ; The sector to read from must be in cl, but our function takes it
    ; in bl
    mov cl, bl

    ; The destination address must be in bx, but our function takes it
    ; in dx
    mov bx, dx

    mov ch, 0x00        ; Cylinder goes in ch
    mov dh, 0x00        ; Cylinder head goes in dh

    ; Store boot drive in dl
    mov dl, byte[boot_drive]

    ; Perform the BIOS disk read
    int 0x13

    ; Check read error
    jc bios_disk_error

    ; Pop number of sectors to read
    ; Compare with sectors actually read
    pop bx
    cmp al, bl
    jne bios_disk_error

    ; Restore the registers
    pop dx
    pop cx
    pop bx
    pop ax

    ; Return
    ret


bios_disk_error:
    ; Infinite loop to hang
    jmp $