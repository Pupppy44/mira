[bits 32]

; Initialize the page table that identity‑maps the first 2 MiB
; and is accessible from ring 3 (for user mode tasks).

init_pt_protected:
    pushad

    ; ------------------------------------------------------------------
    ; 1. Clear 16 KiB starting at 0x1000 and point CR3 at it (PML4)
    ; ------------------------------------------------------------------
    mov     edi, 0x1000          ; PML4 base
    mov     cr3, edi
    xor     eax, eax
    mov     ecx, 4096            ; 4096 dwords = 16 KiB
    rep     stosd

    ; ------------------------------------------------------------------
    ; 2. Build the hierarchy
    ;    We need U/S=1 on every level we want ring‑3 to traverse.
    ;    Flags = P | RW | US  ->  0x7
    ; ------------------------------------------------------------------
    mov     edi, cr3             ; PML4[0]
    mov     dword [edi], 0x2007  ; -> PDPT @ 0x2000, flags 0x7
    add     edi, 0x1000          ; PDPT[0]
    mov     dword [edi], 0x3007  ; -> PDT  @ 0x3000, flags 0x7
    add     edi, 0x1000          ; PDT[0]
    mov     dword [edi], 0x4007  ; -> PT   @ 0x4000, flags 0x7

    ; ------------------------------------------------------------------
    ; 3. Fill PT with 512 4 KiB user pages covering 0‑2 MiB
    ; ------------------------------------------------------------------
    add     edi, 0x1000          ; PT base
    mov     ebx, 0x00000007      ; phys=0x0000 | P|RW|US
    mov     ecx, 512
.add_page:
    mov     dword [edi], ebx
    add     ebx, 0x1000          ; next 4 KiB
    add     edi, 8               ; 64‑bit entry stride
    loop    .add_page

    ; ------------------------------------------------------------------
    ; 4. Turn PAE on (bit 5 of CR4) – 64‑bit long mode needs it.
    ; ------------------------------------------------------------------
    mov     eax, cr4
    or      eax, 1 << 5
    mov     cr4, eax

    ; All set – higher layers enable long mode & paging later.
    popad
    ret