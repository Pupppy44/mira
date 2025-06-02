#include "../inc/syscalls.h"
#include "../inc/util.h"

// Mira System Call Print Function
static long mk_syscall_print(mk_syscall_args *args) {
    const char* string = (const char*)args->arg1;
    
    mk_util_print(string);

    return 0;
}

// Mira System Call Function Definition & Table
typedef long (*mk_syscall_func)(mk_syscall_args *);
static mk_syscall_func const syscall_table[] = {
    NULL, // Unused
    mk_syscall_print // Print to VGA text buffer
};

// Mira System Call Dispatcher
// This function handles the logic for calling the appropriate syscall function
long mk_syscall_dispatch(uint64_t syscall_number, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5) {
    long result = -1; // Default to -1 (invalid syscall or not implemented)

    if (syscall_number < (sizeof syscall_table / sizeof syscall_table[0]) && syscall_table[syscall_number]) {
        mk_syscall_args args = { a1, a2, a3, a4, a5, 0 };
        result = syscall_table[syscall_number](&args);
    }

    return result;
}

// Mira System Call Handler
// This function is purely assembly because it needs to preserve
// registers and handle the syscall interrupt properly.
__attribute__((naked)) void mk_syscall_handler(void) {
    __asm__ __volatile__(
        // Save all registers except RAX (return value)
        "pushq %rax\n\t"
        "pushq %rbx\n\t"
        "pushq %rcx\n\t"
        "pushq %rdx\n\t"
        "pushq %rsi\n\t"
        "pushq %rdi\n\t"
        "pushq %r8\n\t"
        "pushq %r9\n\t"
        "pushq %r10\n\t"

        "mov 64(%rsp), %rdi\n\t" 
        "mov 24(%rsp), %rsi\n\t"
        "mov 32(%rsp), %rdx\n\t"
        "mov 0(%rsp), %rcx\n\t"
        "mov 16(%rsp), %r8\n\t"
        "mov 8(%rsp), %r9\n\t" 

        "call mk_syscall_dispatch\n\t" // Call the syscall dispatcher

        // Restore everything except RAX (return value)
        "popq %r10\n\t"
        "popq %r9\n\t"
        "popq %r8\n\t"
        "popq %rdi\n\t"
        "popq %rsi\n\t"
        "popq %rdx\n\t"
        "popq %rcx\n\t"
        "popq %rbx\n\t"
        "addq $8, %rsp\n\t" // No need to pop RAX, it will be used as return value

        "iretq\n\t" // End of syscall handler, return to user mode
    );
}
