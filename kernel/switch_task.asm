section .text
    global user_regs_switch

user_regs_switch:
    ; Input: Pointer to the register_t structure in rdi (first argument in x64)

    ; Save current context (assuming this is called from an interrupt/exception)
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push rsp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    pushfq          ; Push RFLAGS
    ; Load the new register context
    mov rbp, rdi ; Get the pointer to the register_t struct

    ; Load segment registers (assuming user mode segments are set up in GDT)
    mov ax, 0x23  ; User data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Load general-purpose registers
    mov rax, [rbp + 0]
    mov rbx, [rbp + 8]
    mov rcx, [rbp + 16]
    mov rdx, [rbp + 24]
    mov rsi, [rbp + 32]
    mov rdi, [rbp + 40]
    mov rbp, [rbp + 48]
    mov rsp, [rbp + 56] ; Load user rsp
    mov r8,  [rbp + 64]
    mov r9,  [rbp + 72]
    mov r10, [rbp + 80]
    mov r11, [rbp + 88]
    mov r12, [rbp + 96]
    mov r13, [rbp + 104]
    mov r14, [rbp + 112]
    mov r15, [rbp + 120]

    ; Load rflags (ensure interrupts are enabled if needed)
    push qword [rbp + 128]
    popfq

    ; Load code segment and instruction pointer (iretq will pop these)
    push 0x1b ; User code segment
    push qword [rbp + 136] ; RIP

    ; Restore the saved context by popping in reverse order
    ; (This part might need adjustment depending on how the scheduler is called)
    ; pop rdi ; Pop RIP
    ; pop cs
    ; popfq
    ; pop r15
    ; pop r14
    ; pop r13
    ; pop r12
    ; pop r11
    ; pop r10
    ; pop r9
    ; pop r8
    ; pop rsp
    ; pop rbp
    ; pop rdi
    ; pop rsi
    ; pop rdx
    ; pop rcx
    ; pop rbx
    ; pop rax

    iretq ; Return to the new task