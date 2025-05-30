#ifndef TASKS_H
#define TASKS_H

#include "mem.h"

// Structure for all Mira tasks
typedef struct _mk_task {
    int id;
    const char* name;
    uintptr_t base;
    uintptr_t stack;
    uintptr_t stack_ptr;
    uintptr_t user_stack_base; // For user mode tasks
    uintptr_t user_stack_ptr; // For user mode tasks
    int status; // 0 = Not Running, 1 = Running
    int mode; // 0 = Kernel, 1 = User
} mk_task;

mk_task* mk_create_task(unsigned char* shellcode, size_t shellcode_size, const char* name);

mk_task* mk_create_task_from_function(int (*entry_point)(void), const char* name);

void mk_execute_task(mk_task* task);

void mk_exit_task();

mk_task** mk_get_tasks();

int mk_get_task_count();

#endif