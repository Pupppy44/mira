#include "../include/syscalls.h"

const char* ulongToString(unsigned long num) {
    static char str[21];
    int i = 0;
    do {
        str[i++] = num % 10 + '0';
        num /= 10;
    } while (num > 0);
    str[i] = '\0';

    // Reverse the string
    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }

    return str;
}

void mk_print(const char* str) {
    static uint16_t* terminal_buffer = (uint16_t*)0xb8000;
    static size_t terminal_pos = 0;

    for (size_t i = 0; str[i] != '\0'; i++) {
        terminal_buffer[terminal_pos++] = (0x0F << 8) | str[i];
    }
}

void sys_test(unsigned long status) {
    mk_print("Syscall sys_test called with status: ");
    mk_print(ulongToString(status));
    mk_print("\n");
}

// Syscall table
typedef long (*syscall_func_t)(unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);

syscall_func_t syscall_table[] = {
    NULL,
    (syscall_func_t)sys_test,
};

// The syscall handler
void syscall_handler() {
    registers_t regs;

    // Save the registers
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
    mk_print("Syscall number: ");
    mk_print(ulongToString(syscall_number));
    mk_print("\n");

    if (syscall_number > 0 && syscall_number < sizeof(syscall_table) / sizeof(syscall_table[0]) && syscall_table[syscall_number]) {
        syscall_func_t func = syscall_table[syscall_number];

        // Extract arguments from the registers_t structure
        unsigned long arg1 = regs.rdi;
        unsigned long arg2 = regs.rsi;
        unsigned long arg3 = regs.rdx;
        unsigned long arg4 = regs.r10;
        unsigned long arg5 = regs.r8;
        unsigned long arg6 = regs.r9;

        // Call the syscall implementation
        long result = func(arg1, arg2, arg3, arg4, arg5, arg6);

        // Set the return value in rax (important for user-space syscalls)
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
