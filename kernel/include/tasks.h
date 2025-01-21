#ifndef TASKS_H
#define TASKS_H

#include <stdint.h>
#include <stddef.h>
#include "mem.h"

// Structure for all Mira tasks
typedef struct _mk_task {
    int id;
    uintptr_t base;
    uintptr_t stack;
    uintptr_t stack_ptr;
} mk_task;

mk_task* mk_create_task(unsigned char* shellcode, size_t shellcode_size);

void mk_execute_task(mk_task* task);

void mk_exit_task();

mk_task** mk_get_tasks();

int mk_get_task_count();

#endif