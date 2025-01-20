#ifndef TASKS_H
#define TASKS_H

#include <stdint.h>
#include <stddef.h>
#include "mem.h"

typedef struct register_t {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t useresp;
    uint32_t eflags;
    uint32_t eip;
    uint32_t cr3;
} register_t;


// Structure for all Mira tasks
typedef struct _mk_task {
    int id;
    uintptr_t base;
    uintptr_t stack;
    register_t regs;
} mk_task;

mk_task* mk_create_task(unsigned char* shellcode, size_t shellcode_size);

void mk_execute_task(mk_task* task);

mk_task** mk_get_tasks();

int mk_get_task_count();

#endif