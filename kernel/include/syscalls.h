// In include/syscalls.h
#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    unsigned long r15, r14, r13, r12, r11, r10, r9, r8;
    unsigned long rbp, rdi, rsi, rdx, rcx, rbx, rax;
    unsigned long fs_base, gs_base;
} mk_syscall_registers;

typedef struct {
    unsigned long arg1, arg2, arg3, arg4, arg5, arg6;
} mk_syscall_args;

// Mira Kernel Syscall Handler
void mk_syscall_handler();

#endif