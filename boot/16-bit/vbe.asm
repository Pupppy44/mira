; vbe.asm
; Enable VESA 2.0 display

[bits 16]

; Mira runs at 1280x720 with 32-bit color
TARGET_WIDTH    equ 1280
TARGET_HEIGHT   equ 720
TARGET_BPP      equ 32

BOOT_INFO_ADDR       equ 0x1000      ; Final graphics info for the kernel. The kernel can
                                     ; access the mk_boot_info_t structure at this address.
VBE_INFO_BUFFER      equ 0x2000      ; Temp buffer for the main VBE info block
VBE_MODE_INFO_BUFFER equ 0x2100      ; Temp buffer for specific mode info blocks

; This function searches for and sets a VBE mode matching the target configuration
init_vbe:
    ; Get VBE info block to find the list of supported modes
    mov ax, 0x4F00                  ; VESA function: Get VBE Info
    mov di, VBE_INFO_BUFFER
    int 0x10
    cmp ax, 0x004F                  ; Check for VBE success code (0x004F)
    jne .failure

    ; Get the segment:offset pointer to the mode list from the info block
    mov ax, [VBE_INFO_BUFFER + 16]  ; VBE info block contains segment at offset 16
    mov es, ax
    mov si, [VBE_INFO_BUFFER + 14]  ; VBE info block contains offset at offset 14

.mode_loop:
    ; Read the next 16-bit mode number from the list
    mov ax, [es:si]
    add si, 2                       ; Point to the next entry for the next loop

    ; Check if we have reached the end of the mode list
    cmp ax, 0xFFFF                  ; 0xFFFF marks the end of the list, so
                                    ; check for an error or not.
    je .failure

    ; For the current mode, get the specific information block
    mov cx, ax
    push si                         ; Save our position in the mode list
    push es

    mov ax, 0x4F01                  ; VESA function: Get Mode Info
    mov di, VBE_MODE_INFO_BUFFER
    int 0x10

    pop es                          ; Restore position in the mode list
    pop si
    cmp ax, 0x004F                  ; Check for VBE success code again. If it fails,
                                    ; we just keep looping until we find a valid mode
    jne .mode_loop

    ; Check if this mode matches our requirements
    ; ? cmp compares the current mode's attributes with the target attributes, while
    ; ? jne checks if the compare value is non-zero (meaning the mode is not a match).
    cmp word [VBE_MODE_INFO_BUFFER + 0x12], TARGET_WIDTH
    jne .mode_loop
    cmp word [VBE_MODE_INFO_BUFFER + 0x14], TARGET_HEIGHT
    jne .mode_loop
    cmp byte [VBE_MODE_INFO_BUFFER + 0x19], TARGET_BPP
    jne .mode_loop

    ; If we get here, we have found the correct mode. Now, we can set it!
    jmp .success

.success:
    ; Set the video mode
    mov bx, cx                      ; Move the correct mode number into BX
    or bx, 0x4000                   ; Set bit 14 to enable the Linear Framebuffer,
                                    ; which is needed for our 32-bit color depth.
    mov ax, 0x4F02                  ; VESA function: Set Mode
    int 0x10
    cmp ax, 0x004F
    jne .failure

    ; Copy final info to the mk_boot_info structure for the kernel
    mov esi, VBE_MODE_INFO_BUFFER
    mov edi, BOOT_INFO_ADDR

    mov eax, [esi + 0x28]           ; Framebuffer Address (dword / offset 40)
    mov [edi + 0], eax
    mov dword [edi + 4], 0

    mov ax, [esi + 0x12]            ; Width (word / offset 18)
    mov [edi + 8], ax

    mov ax, [esi + 0x14]            ; Height (word / offset 20)
    mov [edi + 12], ax

    mov ax, [esi + 0x10]            ; Pitch (word / offset 16)
    mov [edi + 16], ax

    mov al, [esi + 0x19]            ; BPP (byte / offset 25)
    mov [edi + 20], al

    ret

.failure:
    ; Hang on interrupt
    cli
    hlt