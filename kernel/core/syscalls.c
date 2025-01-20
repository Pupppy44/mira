#include "../include/syscalls.h"

/*
Syscall Args:
arg1: rdi
arg2: rsi
arg3: rdx
arg4: r10
arg5: r8
arg6: r9
*/

// Syscall table
typedef long (*mk_syscall_function)(mk_syscall_args);

// Syscall implementations
void sys_print(mk_syscall_args args) {
    // Literally all we do is print a char (rdi) at position (rsi) as a color (rdx)
    char* vga = (char*)0xB8000;
    vga[0] = 'B';
    vga[1] = 0x04;
    while (1);
    vga[args.arg2 * 2] = (char)args.arg1;
    vga[args.arg2 * 2 + 1] = (char)args.arg3;
}

void sys_vga(mk_syscall_args args) {
    unsigned char* vga = (unsigned char*)0xA0000;
    unsigned char color = (unsigned char)args.arg1;
    unsigned char pixel = (unsigned char)args.arg2;
    
    vga[pixel] = color;
}

mk_syscall_function syscall_table[] = {
    NULL,
    (mk_syscall_function)sys_print,
};

// Mira Kernel Syscall Handler
void mk_syscall_handler() {
    mk_syscall_registers regs;

    // Grab the registers to use
    __asm__ volatile(
        "mov %%rax, %0\n"
        "mov %%rdi, %1\n"
        "mov %%rsi, %2\n"
        "mov %%rdx, %3\n"
        "mov %%r10, %4\n"
        "mov %%r8, %5\n"
        "mov %%r9, %6\n"
        : "=r"(regs.rax), "=r"(regs.rdi), "=r"(regs.rsi), "=r"(regs.rdx), "=r"(regs.r10), "=r"(regs.r8), "=r"(regs.r9)
    );
  
    unsigned long syscall_number = regs.rax;

    if (syscall_number > 0 && syscall_number < sizeof(syscall_table) / sizeof(syscall_table[0]) && syscall_table[syscall_number]) {
        mk_syscall_function func = syscall_table[syscall_number];

        // Call the syscall implementation
        long result = func((mk_syscall_args){regs.rdi, regs.rsi, regs.rdx, regs.r10, regs.r8, regs.r9});

        // Set the return value in rax
        regs.rax = result;
    }

    // Restore the registers
    __asm__ volatile(
        "mov %0, %%rax\n"
        "mov %1, %%rdi\n"
        "mov %2, %%rsi\n"
        "mov %3, %%rdx\n"
        "mov %4, %%r10\n"
        "mov %5, %%r8\n"
        "mov %6, %%r9\n"
        :
        : "r"(regs.rax), "r"(regs.rdi), "r"(regs.rsi), "r"(regs.rdx), "r"(regs.r10), "r"(regs.r8), "r"(regs.r9)
    );

    // Return from the interrupt
    __asm__ volatile("iretq");
}
